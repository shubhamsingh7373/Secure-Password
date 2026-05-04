/* ═══════════════════════════════════════
   Healix Vault – Frontend Application
   ═══════════════════════════════════════ */

'use strict';

// ── State ──────────────────────────────
let authToken      = '';
let allEntries     = [];
let activeCategory = 'all';
let pendingDeleteId = null;
let generatedPassword = '';
let lockTimerInterval = null;
let lockSecondsLeft  = 300; // 5 minutes

// ── Keyboard shortcuts ─────────────────
document.addEventListener('keydown', e => {
  if ((e.ctrlKey || e.metaKey) && e.key === 'f') {
    e.preventDefault();
    const s = document.getElementById('search-input');
    if (s) s.focus();
  }
  if (e.key === 'Escape') {
    closeModal(); closeGeneratorModal(); closeDeleteModal(); closeSettingsModal();
  }
  resetAutoLock();
});
document.addEventListener('mousemove', resetAutoLock);
document.addEventListener('click',     resetAutoLock);

// ── Auto-lock ──────────────────────────
function resetAutoLock() {
  if (!authToken) return;
  lockSecondsLeft = 300;
}

function startLockTimer() {
  lockSecondsLeft = 300;
  clearInterval(lockTimerInterval);
  lockTimerInterval = setInterval(() => {
    lockSecondsLeft--;
    const m = Math.floor(lockSecondsLeft / 60);
    const s = String(lockSecondsLeft % 60).padStart(2, '0');
    const el = document.getElementById('autolock-label');
    if (el) el.textContent = `${m}:${s}`;
    if (lockSecondsLeft <= 0) { clearInterval(lockTimerInterval); logout(); }
  }, 1000);
}

function stopLockTimer() {
  clearInterval(lockTimerInterval);
}

// ── Toast Notifications ────────────────
function showToast(msg, type = 'info') {
  const container = document.getElementById('toast-container');
  const t = document.createElement('div');
  t.className = `toast toast-${type}`;
  const icon = type === 'success' ? 'fa-circle-check' : type === 'error' ? 'fa-circle-xmark' : 'fa-circle-info';
  t.innerHTML = `<i class="fa-solid ${icon}"></i>${msg}`;
  container.appendChild(t);
  setTimeout(() => {
    t.classList.add('hide');
    setTimeout(() => t.remove(), 300);
  }, 3000);
}

// ── API helpers ────────────────────────
async function api(url, opts = {}) {
  if (!opts.headers) opts.headers = {};
  if (authToken) opts.headers['Authorization'] = authToken;
  if (opts.body && !opts.headers['Content-Type'])
    opts.headers['Content-Type'] = 'application/json';
  const res = await fetch(url, opts);
  if (res.status === 401) { logout(); return null; }
  return res;
}

// ── Check server status ────────────────
async function checkStatus() {
  try {
    const res = await fetch('/api/status');
    const data = await res.json();
    showScreen('auth');
    if (data.isFirstRun) {
      showEl('setup-form'); hideEl('login-form');
    } else {
      hideEl('setup-form'); showEl('login-form');
      setTimeout(() => document.getElementById('login-pwd').focus(), 100);
    }
  } catch {
    showAuthError('Cannot connect to Healix Vault server. Is the backend running?');
  }
}

// ── Auth ───────────────────────────────
function onSetupPwdInput() {
  const val = document.getElementById('setup-pwd1').value;
  updateStrength(val, 'setup-str-fill', 'setup-str-label');
}

async function setupMaster() {
  const p1 = document.getElementById('setup-pwd1').value;
  const p2 = document.getElementById('setup-pwd2').value;
  if (p1.length < 8) return showAuthError('Password must be at least 8 characters.');
  if (p1 !== p2)     return showAuthError('Passwords do not match.');
  try {
    const res = await fetch('/api/setup', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ password: p1 })
    });
    if (res.ok) {
      const d = await res.json();
      authToken = d.token;
      enterDashboard();
    } else {
      showAuthError('Setup failed. Please try again.');
    }
  } catch { showAuthError('Network error.'); }
}

async function login() {
  const pwd = document.getElementById('login-pwd').value;
  if (!pwd) return showAuthError('Please enter your master password.');
  try {
    const res = await fetch('/api/login', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ password: pwd })
    });
    if (res.ok) {
      const d = await res.json();
      authToken = d.token;
      enterDashboard();
    } else {
      showAuthError('Invalid master password. Please try again.');
    }
  } catch { showAuthError('Network error.'); }
}

function enterDashboard() {
  showAuthError('');
  showScreen('dashboard');
  startLockTimer();
  loadEntries();
}

function logout() {
  authToken = '';
  allEntries = [];
  activeCategory = 'all';
  stopLockTimer();
  const lp = document.getElementById('login-pwd');
  if (lp) lp.value = '';
  checkStatus();
}

function showAuthError(msg) {
  document.getElementById('auth-error').textContent = msg;
}

// ── Entries ────────────────────────────
async function loadEntries() {
  try {
    const res = await api('/api/entries');
    if (!res || !res.ok) return;
    allEntries = await res.json();
    buildCategoryNav();
    renderFilteredEntries();
    updateStats();
  } catch (e) { showToast('Failed to load entries.', 'error'); }
}

function buildCategoryNav() {
  const cats = [...new Set(allEntries.map(e => (e.category || '').trim()).filter(Boolean))].sort();
  const navList = document.getElementById('nav-list');
  const count = document.getElementById('nav-all-badge');
  if (count) count.textContent = allEntries.length;

  // Remove old category items (keep first "All Items")
  Array.from(navList.querySelectorAll('.nav-item[data-cat]:not([data-cat="all"])')).forEach(el => el.remove());

  cats.forEach(cat => {
    const c = allEntries.filter(e => (e.category || '') === cat).length;
    const li = document.createElement('li');
    li.className = 'nav-item' + (activeCategory === cat ? ' active' : '');
    li.dataset.cat = cat;
    li.onclick = () => filterByCategory(cat, li);
    li.innerHTML = `<i class="fa-solid fa-folder"></i><span>${cat}</span><span class="nav-badge">${c}</span>`;
    navList.appendChild(li);
  });

  // Update datalist for category input
  const dl = document.getElementById('cat-datalist');
  if (dl) dl.innerHTML = cats.map(c => `<option value="${c}">`).join('');
}

function filterByCategory(cat, el) {
  activeCategory = cat;
  document.querySelectorAll('.nav-item').forEach(n => n.classList.remove('active'));
  if (el) el.classList.add('active');
  const title = cat === 'all' ? 'All Items' : cat;
  document.getElementById('view-title').textContent = title;
  renderFilteredEntries();
}

function doFilter() {
  renderFilteredEntries();
}

function renderFilteredEntries() {
  const q = (document.getElementById('search-input').value || '').toLowerCase();
  let list = activeCategory === 'all' ? allEntries
    : allEntries.filter(e => (e.category || '').toLowerCase() === activeCategory.toLowerCase());
  if (q) {
    list = list.filter(e =>
      (e.website  || '').toLowerCase().includes(q) ||
      (e.username || '').toLowerCase().includes(q) ||
      (e.category || '').toLowerCase().includes(q) ||
      (e.notes    || '').toLowerCase().includes(q)
    );
  }
  renderEntries(list);
}

function renderEntries(entries) {
  const container = document.getElementById('entries-container');
  const empty     = document.getElementById('empty-state');
  container.innerHTML = '';

  const countLabel = document.getElementById('view-count');
  if (countLabel) countLabel.textContent = `${entries.length} entr${entries.length===1?'y':'ies'}`;

  if (entries.length === 0) {
    showEl('empty-state'); return;
  }
  hideEl('empty-state');

  entries.forEach(e => {
    const div = document.createElement('div');
    div.className = 'entry-card';
    const letter = (e.website || '?')[0].toUpperCase();
    const color  = avatarColor(e.website || '');
    div.innerHTML = `
      <div class="entry-top">
        <div class="entry-avatar" style="background:${color}">${letter}</div>
        <div class="entry-meta">
          <div class="entry-site">${esc(e.website || 'Unnamed')}</div>
          <div class="entry-user">${esc(e.username || '')}</div>
        </div>
        <span class="entry-cat">${esc(e.category || 'General')}</span>
      </div>
      <div class="entry-pwd-row">
        <span class="entry-pwd-dots" id="dots-${e.id}">••••••••••••</span>
        <span class="entry-pwd-text" id="text-${e.id}">${esc(e.password || '')}</span>
        <button class="icon-btn" title="Show/Hide" onclick="toggleCardPwd(${e.id})">
          <i class="fa-solid fa-eye" id="eye-${e.id}"></i>
        </button>
      </div>
      <div class="entry-actions">
        <button onclick="copyToClipboard('${(e.password||'').replace(/'/g,"\\'")}','Password')">
          <i class="fa-solid fa-copy"></i> Copy
        </button>
        <button onclick="editEntry(${e.id})">
          <i class="fa-solid fa-pen"></i> Edit
        </button>
        <button class="btn-del" onclick="openDeleteModal(${e.id},'${esc(e.website||'this entry')}')">
          <i class="fa-solid fa-trash"></i>
        </button>
      </div>`;
    container.appendChild(div);
  });
}

function toggleCardPwd(id) {
  const dots = document.getElementById(`dots-${id}`);
  const text  = document.getElementById(`text-${id}`);
  const eye   = document.getElementById(`eye-${id}`);
  const showing = text.style.display === 'block';
  dots.style.display = showing ? 'block' : 'none';
  text.style.display = showing ? 'none'  : 'block';
  eye.className = showing ? 'fa-solid fa-eye' : 'fa-solid fa-eye-slash';
}

function updateStats() {
  document.getElementById('stat-total').textContent = allEntries.length;
  const cats = new Set(allEntries.map(e => (e.category||'').trim()).filter(Boolean));
  document.getElementById('stat-cats').textContent  = cats.size;
}

// ── Add / Edit Modal ───────────────────
function openAddModal() {
  resetModal('Add Entry');
  showEl('entry-modal');
}

async function editEntry(id) {
  try {
    const res = await api(`/api/entries/${id}`);
    if (!res || !res.ok) return;
    const e = await res.json();
    resetModal('Edit Entry');
    document.getElementById('entry-id').value       = e.id;
    document.getElementById('entry-website').value  = e.website  || '';
    document.getElementById('entry-username').value = e.username || '';
    document.getElementById('entry-password').value = e.password || '';
    document.getElementById('entry-category').value = e.category || '';
    document.getElementById('entry-notes').value    = e.notes    || '';
    updateStrength(e.password || '', 'modal-str-fill', 'modal-str-label');
    showEl('entry-modal');
  } catch { showToast('Failed to load entry.', 'error'); }
}

function resetModal(title) {
  document.getElementById('modal-title').textContent = title;
  ['entry-id','entry-website','entry-username','entry-password','entry-category','entry-notes']
    .forEach(id => document.getElementById(id).value = '');
  updateStrength('', 'modal-str-fill', 'modal-str-label');
}

function closeModal() {
  hideEl('entry-modal');
}

async function saveEntry() {
  const id  = document.getElementById('entry-id').value;
  const data = {
    website:  document.getElementById('entry-website').value.trim(),
    username: document.getElementById('entry-username').value.trim(),
    password: document.getElementById('entry-password').value,
    category: document.getElementById('entry-category').value.trim(),
    notes:    document.getElementById('entry-notes').value.trim()
  };
  if (!data.website)  return showToast('Website is required.', 'error');
  if (!data.username) return showToast('Username is required.', 'error');
  if (!data.password) return showToast('Password is required.', 'error');

  const method = id ? 'PUT' : 'POST';
  const url    = id ? `/api/entries/${id}` : '/api/entries';
  try {
    const res = await api(url, { method, body: JSON.stringify(data) });
    if (res && res.ok) {
      closeModal();
      await loadEntries();
      showToast(id ? 'Entry updated.' : 'Entry saved.', 'success');
    } else {
      showToast('Failed to save entry.', 'error');
    }
  } catch { showToast('Network error.', 'error'); }
}

// ── Delete Modal ───────────────────────
function openDeleteModal(id, name) {
  pendingDeleteId = id;
  document.getElementById('delete-entry-name').textContent = name;
  showEl('delete-modal');
}

function closeDeleteModal() { hideEl('delete-modal'); pendingDeleteId = null; }

async function confirmDelete() {
  if (!pendingDeleteId) return;
  try {
    const res = await api(`/api/entries/${pendingDeleteId}`, { method: 'DELETE' });
    if (res && res.ok) {
      closeDeleteModal();
      await loadEntries();
      showToast('Entry deleted.', 'success');
    } else {
      showToast('Failed to delete.', 'error');
    }
  } catch { showToast('Network error.', 'error'); }
}

// ── Password Generator ─────────────────
function openGeneratorModal() {
  showEl('generator-modal');
  generatePassword();
}

function closeGeneratorModal() { hideEl('generator-modal'); }

function generatePassword() {
  const len     = parseInt(document.getElementById('gen-length').value, 10);
  const upper   = document.getElementById('opt-upper').checked;
  const lower   = document.getElementById('opt-lower').checked;
  const numbers = document.getElementById('opt-numbers').checked;
  const symbols = document.getElementById('opt-symbols').checked;

  let chars = '';
  if (upper)   chars += 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
  if (lower)   chars += 'abcdefghijklmnopqrstuvwxyz';
  if (numbers) chars += '0123456789';
  if (symbols) chars += '!@#$%^&*()-_=+[]{}|;:,.<>?';
  if (!chars)  chars  = 'abcdefghijklmnopqrstuvwxyz';

  const arr = new Uint32Array(len);
  crypto.getRandomValues(arr);
  generatedPassword = Array.from(arr).map(v => chars[v % chars.length]).join('');

  document.getElementById('gen-output').textContent = generatedPassword;
  updateStrength(generatedPassword, 'gen-str-fill', 'gen-str-label');
}

function copyGenPassword() {
  if (!generatedPassword) return;
  copyToClipboard(generatedPassword, 'Password');
}

function useGeneratedPassword() {
  if (!generatedPassword) return;
  document.getElementById('entry-password').value = generatedPassword;
  updateStrength(generatedPassword, 'modal-str-fill', 'modal-str-label');
  closeGeneratorModal();
  showEl('entry-modal');
}

function quickGenerate() {
  // Inline quick-generate for the modal
  const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*';
  const arr   = new Uint32Array(20);
  crypto.getRandomValues(arr);
  const pwd = Array.from(arr).map(v => chars[v % chars.length]).join('');
  document.getElementById('entry-password').value = pwd;
  updateStrength(pwd, 'modal-str-fill', 'modal-str-label');
  showToast('Password generated!', 'info');
}

// ── Password Strength ──────────────────
function updateStrength(pwd, fillId, labelId) {
  const fill  = document.getElementById(fillId);
  const label = document.getElementById(labelId);
  if (!fill || !label) return;

  let score = 0;
  if (!pwd) { fill.style.width = '0'; fill.className = 'strength-fill'; label.textContent = ''; return; }
  if (pwd.length >= 8)  score++;
  if (pwd.length >= 12) score++;
  if (/[A-Z]/.test(pwd)) score++;
  if (/[a-z]/.test(pwd)) score++;
  if (/[0-9]/.test(pwd)) score++;
  if (/[^A-Za-z0-9]/.test(pwd)) score++;

  const levels = [
    { min:0, cls:'str-weak',   lbl:'Weak',   w:'20%'  },
    { min:2, cls:'str-weak',   lbl:'Weak',   w:'30%'  },
    { min:3, cls:'str-fair',   lbl:'Fair',   w:'50%'  },
    { min:4, cls:'str-good',   lbl:'Good',   w:'70%'  },
    { min:5, cls:'str-strong', lbl:'Strong', w:'100%' },
  ];
  let lvl = levels[0];
  for (const l of levels) if (score >= l.min) lvl = l;

  fill.className      = `strength-fill ${lvl.cls}`;
  fill.style.width    = lvl.w;
  label.textContent   = lvl.lbl;
  label.style.color   = lvl.cls === 'str-strong' ? '#10b981'
                      : lvl.cls === 'str-good'   ? '#3b82f6'
                      : lvl.cls === 'str-fair'   ? '#f59e0b' : '#ef4444';
}

function onSetupPwdInput() {
  updateStrength(document.getElementById('setup-pwd1').value, 'setup-str-fill', 'setup-str-label');
}

// ── Clipboard ──────────────────────────
function copyToClipboard(text, label = 'Text') {
  if (!text) return;
  navigator.clipboard.writeText(text)
    .then(() => showToast(`${label} copied to clipboard!`, 'success'))
    .catch(() => showToast('Copy failed.', 'error'));
}

// ── Visibility toggle ──────────────────
function toggleVis(inputId, btn) {
  const el = document.getElementById(inputId);
  const icon = btn.querySelector('i');
  if (el.type === 'password') {
    el.type = 'text';
    if (icon) { icon.classList.remove('fa-eye'); icon.classList.add('fa-eye-slash'); }
  } else {
    el.type = 'password';
    if (icon) { icon.classList.remove('fa-eye-slash'); icon.classList.add('fa-eye'); }
  }
}

// ── Settings & Misc ────────────────────
function openSettingsModal() { showEl('settings-modal'); }
function closeSettingsModal() { hideEl('settings-modal'); }

function clearClipboard() {
  navigator.clipboard.writeText('')
    .then(() => showToast('Clipboard cleared securely.', 'success'))
    .catch(() => showToast('Failed to clear clipboard.', 'error'));
}

// ── Overlay click outside ──────────────
function handleOverlayClick(e, modalId) {
  if (e.target.id === modalId) {
    if (modalId === 'entry-modal')     closeModal();
    if (modalId === 'generator-modal') closeGeneratorModal();
    if (modalId === 'delete-modal')    closeDeleteModal();
    if (modalId === 'settings-modal')  closeSettingsModal();
  }
}

// ── Screen helpers ─────────────────────
function showScreen(name) {
  const authEl  = document.getElementById('auth-screen');
  const dashEl  = document.getElementById('dashboard-screen');
  if (name === 'auth') {
    authEl.classList.add('active');    authEl.classList.remove('hidden');
    dashEl.classList.remove('active'); dashEl.classList.add('hidden');
  } else {
    dashEl.classList.add('active');    dashEl.classList.remove('hidden');
    authEl.classList.remove('active'); authEl.classList.add('hidden');
  }
}

function showEl(id) { const e = document.getElementById(id); if(e){e.classList.remove('hidden');} }
function hideEl(id) { const e = document.getElementById(id); if(e){e.classList.add('hidden');} }

// ── Utilities ──────────────────────────
function esc(str) {
  return String(str).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;')
    .replace(/"/g,'&quot;').replace(/'/g,'&#039;');
}

const AVATAR_COLORS = [
  'linear-gradient(135deg,#6366f1,#8b5cf6)',
  'linear-gradient(135deg,#10b981,#059669)',
  'linear-gradient(135deg,#f59e0b,#d97706)',
  'linear-gradient(135deg,#ef4444,#dc2626)',
  'linear-gradient(135deg,#3b82f6,#2563eb)',
  'linear-gradient(135deg,#ec4899,#db2777)',
  'linear-gradient(135deg,#14b8a6,#0d9488)',
  'linear-gradient(135deg,#f97316,#ea580c)',
];

function avatarColor(str) {
  let hash = 0;
  for (let i = 0; i < str.length; i++) hash = str.charCodeAt(i) + ((hash << 5) - hash);
  return AVATAR_COLORS[Math.abs(hash) % AVATAR_COLORS.length];
}

// ── Boot ───────────────────────────────
checkStatus();
