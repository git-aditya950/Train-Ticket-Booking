# TrainTrack Frontend-Backend Integration Guide

## Table of Contents
1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Backend Setup](#backend-setup)
4. [Frontend Modifications](#frontend-modifications)
5. [API Integration Points](#api-integration-points)
6. [Testing Integration](#testing-integration)
7. [Troubleshooting](#troubleshooting)
8. [Production Deployment](#production-deployment)

---

## Overview

This guide explains how to integrate the TrainTrack HTML frontend with your C++ backend server. The integration involves:

1. Removing Firebase dependencies
2. Updating API calls to point to C++ backend
3. Implementing authentication with JWT tokens
4. Handling responses from backend
5. Testing the complete system

---

## Prerequisites

### Backend Requirements
- C++ backend server running on `http://localhost:18080`
- All API endpoints implemented as per `BACKEND_REQUIREMENTS.md`
- CORS enabled for frontend requests
- Database initialized with train data

### Frontend Requirements
- Modern web browser (Chrome, Firefox, Safari, Edge)
- Basic understanding of JavaScript
- Text editor or IDE
- Local development server (optional: Python's `http.server`, Node's `http-server`, or VS Code Live Server)

---

## Backend Setup

### 1. Verify Backend is Running

Start your C++ backend server and verify it's accessible:

```bash
# Test server health
curl http://localhost:18080/api/health

# Expected response:
# {"status": "ok", "timestamp": "2025-11-17T10:30:00Z"}
```

### 2. Verify CORS is Enabled

Your C++ backend should include CORS headers in all responses:

```cpp
response.headers["Access-Control-Allow-Origin"] = "*";
response.headers["Access-Control-Allow-Methods"] = "GET, POST, PUT, DELETE, OPTIONS";
response.headers["Access-Control-Allow-Headers"] = "Content-Type, Authorization";
```

Test CORS:

```bash
curl -H "Origin: http://localhost" \
     -H "Access-Control-Request-Method: POST" \
     -H "Access-Control-Request-Headers: Content-Type, Authorization" \
     -X OPTIONS \
     http://localhost:18080/api/auth/login
```

### 3. Initialize Database with Sample Data

Ensure your backend has been initialized with:
- Sample trains (at least 20-30 trains)
- Station data
- Train availability (classes, prices, seats)

---

## Frontend Modifications

### Step 1: Remove Firebase Code

Open `index.html` and locate the Firebase imports section (around line 684-713):

**BEFORE:**
```javascript
// =================================================================================
// 1. FIREBASE & GEMINI SDK IMPORTS
// =================================================================================
import { initializeApp } from "https://www.gstatic.com/firebasejs/11.6.1/firebase-app.js";
import { 
    getAuth, 
    signInAnonymously, 
    signInWithCustomToken, 
    onAuthStateChanged, 
    createUserWithEmailAndPassword, 
    signInWithEmailAndPassword, 
    signOut,
    updateProfile
} from "https://www.gstatic.com/firebasejs/11.6.1/firebase-auth.js";
import { 
    getFirestore, 
    doc, 
    getDoc, 
    addDoc, 
    setDoc, 
    updateDoc, 
    deleteDoc, 
    onSnapshot, 
    collection, 
    query, 
    where, 
    getDocs,
    setLogLevel
} from "https://www.gstatic.com/firebasejs/11.6.1/firebase-firestore.js";
```

**AFTER:**
```javascript
// =================================================================================
// 1. CONFIGURATION
// =================================================================================
// Firebase removed - using C++ backend
```

---

### Step 2: Update Configuration Section

Replace the configuration section (around line 718-730):

**BEFORE:**
```javascript
// --- Firebase Config ---
const firebaseConfig = JSON.parse(
    typeof __firebase_config !== 'undefined' ? __firebase_config : '{"apiKey": "YOUR_API_KEY", "authDomain": "YOUR_AUTH_DOMAIN", "projectId": "YOUR_PROJECT_ID"}'
);
const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';

// --- Gemini API Config ---
const GEMINI_API_KEY = "";
const GEMINI_API_URL = `https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash-preview-09-2025:generateContent?key=${GEMINI_API_KEY}`;
const CXX_BACKEND_URL = "http://localhost:18080/api/search";
```

**AFTER:**
```javascript
// --- Backend API Configuration ---
const API_BASE_URL = "http://localhost:18080/api";
const API_ENDPOINTS = {
    // Auth endpoints
    register: `${API_BASE_URL}/auth/register`,
    login: `${API_BASE_URL}/auth/login`,
    logout: `${API_BASE_URL}/auth/logout`,
    profile: `${API_BASE_URL}/auth/profile`,
    
    // Train endpoints
    search: `${API_BASE_URL}/search`,
    stations: `${API_BASE_URL}/stations`,
    
    // Booking endpoints
    bookings: `${API_BASE_URL}/bookings`
};

// --- Gemini API Config (Optional - keep if using AI features) ---
const GEMINI_API_KEY = "";
const GEMINI_API_URL = `https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash-preview-09-2025:generateContent?key=${GEMINI_API_KEY}`;
```

---

### Step 3: Update App State Variables

Replace the app state section (around line 733-742):

**BEFORE:**
```javascript
// --- App State ---
let db = null;
let auth = null;
let userId = null;
let unsubscribeBookings = null;
let currentBooking = {
    train: null,
    passengers: [],
    selectedClass: null,
    totalFare: 0
};
```

**AFTER:**
```javascript
// --- App State ---
let currentUser = null;  // Stores logged-in user data
let authToken = null;    // JWT token
let userId = null;
let currentBooking = {
    train: null,
    passengers: [],
    selectedClass: null,
    totalFare: 0
};
```

---

### Step 4: Create API Helper Functions

Add these helper functions after the app state section:

```javascript
// =================================================================================
// API HELPER FUNCTIONS
// =================================================================================

/**
 * Makes an authenticated API request
 * @param {string} url - API endpoint URL
 * @param {object} options - Fetch options (method, body, etc.)
 * @returns {Promise<object>} - Response data
 */
async function apiRequest(url, options = {}) {
    // Add default headers
    const headers = {
        'Content-Type': 'application/json',
        ...options.headers
    };
    
    // Add auth token if available
    if (authToken) {
        headers['Authorization'] = `Bearer ${authToken}`;
    }
    
    // Make request
    const response = await fetch(url, {
        ...options,
        headers
    });
    
    // Parse response
    const data = await response.json();
    
    // Handle errors
    if (!response.ok) {
        throw new Error(data.message || 'Request failed');
    }
    
    return data;
}

/**
 * Stores authentication token in localStorage
 */
function saveAuthToken(token) {
    authToken = token;
    localStorage.setItem('authToken', token);
}

/**
 * Loads authentication token from localStorage
 */
function loadAuthToken() {
    const token = localStorage.getItem('authToken');
    if (token) {
        authToken = token;
        return true;
    }
    return false;
}

/**
 * Clears authentication token
 */
function clearAuthToken() {
    authToken = null;
    localStorage.removeItem('authToken');
}

/**
 * Stores user data in localStorage
 */
function saveUserData(user) {
    currentUser = user;
    userId = user.userId;
    localStorage.setItem('currentUser', JSON.stringify(user));
}

/**
 * Loads user data from localStorage
 */
function loadUserData() {
    const userData = localStorage.getItem('currentUser');
    if (userData) {
        currentUser = JSON.parse(userData);
        userId = currentUser.userId;
        return true;
    }
    return false;
}

/**
 * Clears user data
 */
function clearUserData() {
    currentUser = null;
    userId = null;
    localStorage.removeItem('currentUser');
}
```

---

### Step 5: Replace Firebase Initialization

Replace the `initializeAppCore()` function (around line 862-920):

**BEFORE:**
```javascript
async function initializeAppCore() {
    try {
        // Firebase initialization code...
    } catch (error) {
        console.error("Critical Error initializing app:", error);
        showAppInitializationError("Could not initialize the application. Please Refresh.");
    }
}
```

**AFTER:**
```javascript
async function initializeAppCore() {
    try {
        console.log("Initializing TrainTrack application...");
        
        // Load auth token and user data from localStorage
        const hasToken = loadAuthToken();
        const hasUser = loadUserData();
        
        if (hasToken && hasUser) {
            console.log("Found stored authentication");
            
            // Verify token is still valid by fetching profile
            try {
                const response = await apiRequest(API_ENDPOINTS.profile);
                
                if (response.status === 'success') {
                    // Token is valid, update UI
                    currentUser = response.data;
                    userId = currentUser.userId;
                    updateUIForLoggedInUser(currentUser);
                    
                    // Load user's bookings
                    await loadUserBookings();
                } else {
                    // Token invalid, clear and show logged out state
                    console.warn("Stored token is invalid");
                    clearAuthToken();
                    clearUserData();
                    updateUIForLoggedOutUser();
                }
            } catch (error) {
                console.error("Error verifying token:", error);
                clearAuthToken();
                clearUserData();
                updateUIForLoggedOutUser();
            }
        } else {
            // No stored auth, show logged out state
            console.log("No stored authentication found");
            updateUIForLoggedOutUser();
        }
        
        console.log("Application initialized successfully");
        
    } catch (error) {
        console.error("Critical Error initializing app:", error);
        showAppInitializationError("Could not initialize the application. Please check your connection and refresh.");
    }
}
```

---

### Step 6: Update Authentication Functions

Replace the authentication handler functions:

#### **handleLogin**

**BEFORE:**
```javascript
async function handleLogin(e) {
    e.preventDefault();
    const email = document.getElementById('login-email').value;
    const password = document.getElementById('login-password').value;
    try {
        await signInWithEmailAndPassword(auth, email, password);
        closeModal(loginModal);
    } catch (error) {
        alert(`Login Failed: ${error.message}`);
    }
}
```

**AFTER:**
```javascript
async function handleLogin(e) {
    e.preventDefault();
    const email = document.getElementById('login-email').value;
    const password = document.getElementById('login-password').value;
    
    try {
        // Call backend API
        const response = await apiRequest(API_ENDPOINTS.login, {
            method: 'POST',
            body: JSON.stringify({ email, password })
        });
        
        if (response.status === 'success') {
            // Save token and user data
            saveAuthToken(response.token);
            saveUserData(response.data);
            
            // Update UI
            updateUIForLoggedInUser(response.data);
            
            // Load bookings
            await loadUserBookings();
            
            // Close modal
            closeModal(loginModal);
            
            console.log("Login successful");
        } else {
            throw new Error(response.message || 'Login failed');
        }
    } catch (error) {
        console.error("Login error:", error);
        alert(`Login Failed: ${error.message}`);
    }
}
```

#### **handleRegister**

**BEFORE:**
```javascript
async function handleRegister(e) {
    e.preventDefault();
    const name = document.getElementById('register-name').value;
    const email = document.getElementById('register-email').value;
    const password = document.getElementById('register-password').value;
    try {
        const userCredential = await createUserWithEmailAndPassword(auth, email, password);
        await updateProfile(userCredential.user, { displayName: name });
        closeModal(registerModal);
    } catch (error) {
        alert(`Registration Failed: ${error.message}`);
    }
}
```

**AFTER:**
```javascript
async function handleRegister(e) {
    e.preventDefault();
    const name = document.getElementById('register-name').value;
    const email = document.getElementById('register-email').value;
    const password = document.getElementById('register-password').value;
    
    try {
        // Call backend API
        const response = await apiRequest(API_ENDPOINTS.register, {
            method: 'POST',
            body: JSON.stringify({ name, email, password })
        });
        
        if (response.status === 'success') {
            // Save token and user data
            saveAuthToken(response.token);
            saveUserData(response.data);
            
            // Update UI
            updateUIForLoggedInUser(response.data);
            
            // Close modal
            closeModal(registerModal);
            
            console.log("Registration successful");
            
            // Show success message
            alert("Account created successfully! Welcome to TrainTrack.");
        } else {
            throw new Error(response.message || 'Registration failed');
        }
    } catch (error) {
        console.error("Registration error:", error);
        alert(`Registration Failed: ${error.message}`);
    }
}
```

#### **handleLogout**

**BEFORE:**
```javascript
async function handleLogout() {
    try {
        await signOut(auth);
        await signInAnonymously(auth);
        updateUIForLoggedOutUser();
    } catch (error) {
        console.error("Logout Failed:", error);
    }
}
```

**AFTER:**
```javascript
async function handleLogout() {
    try {
        // Call backend API (optional - JWT can be invalidated client-side)
        if (authToken) {
            try {
                await apiRequest(API_ENDPOINTS.logout, {
                    method: 'POST'
                });
            } catch (error) {
                console.warn("Logout API call failed, clearing local data anyway");
            }
        }
        
        // Clear local data
        clearAuthToken();
        clearUserData();
        
        // Update UI
        updateUIForLoggedOutUser();
        
        console.log("Logout successful");
    } catch (error) {
        console.error("Logout Failed:", error);
    }
}
```

#### **handleProfileUpdate**

**BEFORE:**
```javascript
async function handleProfileUpdate(e) {
    e.preventDefault();
    alert("Profile update is not implemented in this demo.");
}
```

**AFTER:**
```javascript
async function handleProfileUpdate(e) {
    e.preventDefault();
    
    const name = document.getElementById('profile-name').value;
    const phone = document.getElementById('profile-phone').value;
    const newPassword = document.getElementById('profile-password').value;
    
    try {
        // Build update payload
        const updates = { name, phone };
        
        // Add password if provided
        if (newPassword && newPassword.trim() !== '') {
            const currentPassword = prompt("Please enter your current password to change it:");
            if (!currentPassword) {
                alert("Current password required to change password");
                return;
            }
            updates.currentPassword = currentPassword;
            updates.newPassword = newPassword;
        }
        
        // Call backend API
        const response = await apiRequest(API_ENDPOINTS.profile, {
            method: 'PUT',
            body: JSON.stringify(updates)
        });
        
        if (response.status === 'success') {
            // Update stored user data
            saveUserData(response.data);
            
            // Update UI
            document.getElementById('profile-name').value = response.data.name;
            document.getElementById('profile-phone').value = response.data.phone || '';
            document.getElementById('profile-password').value = '';
            
            alert("Profile updated successfully!");
        } else {
            throw new Error(response.message || 'Update failed');
        }
    } catch (error) {
        console.error("Profile update error:", error);
        alert(`Update Failed: ${error.message}`);
    }
}
```

---

### Step 7: Update Train Search Function

Replace the `handleTrainSearch()` function:

**BEFORE:**
```javascript
async function handleTrainSearch(e) {
    e.preventDefault();
    showPage('page-search-results');
    
    const fromValue = document.getElementById('from-station').value || 'New Delhi (NDLS)';
    const toValue = document.getElementById('to-station').value || 'Mumbai (CST)';
    
    // ... loading UI ...
    
    let trainData = [];
    try {
        const url = `${CXX_BACKEND_URL}?from=${encodeURIComponent(fromValue)}&to=${encodeURIComponent(toValue)}`;
        const response = await fetch(url);
        // ... rest of code
    } catch (error) {
        console.warn(`Could not fetch from C++ backend. Using local mock data.`);
        trainData = getRandomTrainsFromMock(fromValue, toValue);
    }
    
    displayTrainResults(trainData);
}
```

**AFTER:**
```javascript
async function handleTrainSearch(e) {
    e.preventDefault();
    showPage('page-search-results');
    
    const fromValue = document.getElementById('from-station').value || 'New Delhi (NDLS)';
    const toValue = document.getElementById('to-station').value || 'Mumbai (CST)';
    const journeyDate = document.getElementById('journey-date').value;
    
    const resultsContainer = document.getElementById('search-results-container');
    resultsContainer.innerHTML = `
        <div class="text-center py-12">
            <svg class="animate-spin h-8 w-8 text-indigo-600 mx-auto" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24">
                <circle class="opacity-25" cx="12" cy="12" r="10" stroke="currentColor" stroke-width="4"></circle>
                <path class="opacity-75" fill="currentColor" d="M4 12a8 8 0 018-8V0C5.373 0 0 5.373 0 12h4zm2 5.291A7.962 7.962 0 014 12H0c0 3.042 1.135 5.824 3 7.938l3-2.647z"></path>
            </svg>
            <p class="mt-4 text-gray-600">Searching for trains...</p>
        </div>`;

    let trainData = [];
    try {
        // Build search URL
        const url = `${API_ENDPOINTS.search}?from=${encodeURIComponent(fromValue)}&to=${encodeURIComponent(toValue)}&date=${encodeURIComponent(journeyDate)}`;
        
        // Call backend API (no auth required for search)
        const response = await apiRequest(url, { method: 'GET' });
        
        if (response.status === 'success') {
            trainData = response.trains || [];
            console.log(`Found ${trainData.length} trains`);
        } else {
            throw new Error(response.message || 'Search failed');
        }
    } catch (error) {
        console.error("Train search error:", error);
        
        // Fallback to mock data
        console.warn("Using local mock data as fallback");
        trainData = getRandomTrainsFromMock(fromValue, toValue);
    }
    
    displayTrainResults(trainData);
}
```

---

### Step 8: Update Booking Functions

Replace the `handlePayment()` function:

**BEFORE:**
```javascript
async function handlePayment(e) {
    e.preventDefault();
    
    if (!userId) {
        alert("You have been logged out. Please log in again.");
        openModal(loginModal);
        return;
    }
    
    const pnr = `${Math.floor(100 + Math.random() * 900)}-${Math.floor(1000000 + Math.random() * 9000000)}`;
    const passengersWithSeats = assignSeats(currentBooking.passengers, currentBooking.selectedClass.class);
    currentBooking.passengers = passengersWithSeats;
    const dateInput = document.getElementById('journey-date');
    currentBooking.journeyDate = dateInput.value;
    
    try {
        if (!db) {
            console.warn("Database not initialized. Simulating booking save.");
        } else {
            // Firebase save code...
        }
        
        // Update confirmation page...
        showPage('page-confirmation');
    } catch (error) {
        console.error("Error saving booking: ", error);
        alert("There was an error saving your booking.");
    }
}
```

**AFTER:**
```javascript
async function handlePayment(e) {
    e.preventDefault();
    
    // Check authentication
    if (!userId || !authToken) {
        alert("You have been logged out. Please log in again to complete your booking.");
        openModal(loginModal);
        return;
    }
    
    // Get journey date
    const dateInput = document.getElementById('journey-date');
    currentBooking.journeyDate = dateInput.value;
    
    try {
        // Prepare booking data for backend
        const bookingData = {
            train: {
                trainNumber: currentBooking.train.trainNumber,
                trainName: currentBooking.train.trainName,
                from: currentBooking.train.from,
                to: currentBooking.train.to,
                departureTime: currentBooking.train.departureTime,
                arrivalTime: currentBooking.train.arrivalTime,
                duration: currentBooking.train.duration
            },
            selectedClass: {
                class: currentBooking.selectedClass.class,
                price: currentBooking.selectedClass.price
            },
            journeyDate: currentBooking.journeyDate,
            passengers: currentBooking.passengers.map(p => ({
                name: p.name,
                age: parseInt(p.age),
                gender: p.gender,
                berth: p.berth
            }))
        };
        
        console.log("Creating booking:", bookingData);
        
        // Call backend API
        const response = await apiRequest(API_ENDPOINTS.bookings, {
            method: 'POST',
            body: JSON.stringify(bookingData)
        });
        
        if (response.status === 'success') {
            // Backend returns booking with PNR and assigned seats
            const booking = response.data;
            
            // Update current booking with backend response
            currentBooking.pnr = booking.pnr;
            currentBooking.passengers = booking.passengers; // Has assigned seats
            currentBooking.bookingId = booking.bookingId;
            
            console.log("Booking confirmed:", booking);
            
            // Update confirmation page
            document.getElementById('confirmation-pnr').textContent = booking.pnr;
            document.getElementById('confirmation-journey-summary').innerHTML = `
                <p class="font-semibold text-gray-800">${booking.train.trainName} (${booking.train.trainNumber})</p>
                <p class="text-sm text-gray-600">${booking.train.from} &rarr; ${booking.train.to}</p>
                <p class="text-sm text-gray-600">Date: ${booking.journeyDate}</p>
                <p class="text-sm text-gray-600">Class: <span class="font-medium">${booking.selectedClass.class}</span> | Total Fare: <span class="font-medium">${formatCurrency(booking.totalFare)}</span></p>
            `;
            document.getElementById('confirmation-passenger-summary').innerHTML = `
                <p class="font-semibold text-gray-800 mt-4">${booking.passengers.length} Passenger(s)</p>
                <ul class="list-disc list-inside text-sm text-gray-600">
                    ${booking.passengers.map(p => `<li>${p.name} (${p.age}) - <span class="font-semibold text-indigo-700">${p.assignedSeat} / ${p.assignedBerth}</span></li>`).join('')}
                </ul>
            `;
            
            // Show confirmation page
            showPage('page-confirmation');
            
            // Reload bookings list
            await loadUserBookings();
            
        } else {
            throw new Error(response.message || 'Booking failed');
        }
        
    } catch (error) {
        console.error("Error creating booking:", error);
        alert(`Booking Failed: ${error.message}`);
    }
}
```

---

### Step 9: Update Bookings Display

Replace the `attachBookingsListener()` function with `loadUserBookings()`:

**BEFORE:**
```javascript
function attachBookingsListener(uid) {
    if (unsubscribeBookings) {
        unsubscribeBookings();
    }
    
    if (!db) {
        console.warn("Database not initialized.");
        displayUserBookings([]);
        return;
    }

    const bookingCollectionPath = `artifacts/${appId}/users/${uid}/bookings`;
    const q = query(collection(db, bookingCollectionPath));
    
    unsubscribeBookings = onSnapshot(q, (snapshot) => {
        const bookings = [];
        snapshot.forEach((doc) => {
            bookings.push({ id: doc.id, ...doc.data() });
        });
        bookings.sort((a, b) => new Date(b.bookingDate) - new Date(a.bookingDate));
        displayUserBookings(bookings);
    });
}
```

**AFTER:**
```javascript
async function loadUserBookings() {
    if (!userId || !authToken) {
        console.log("No user logged in, clearing bookings");
        displayUserBookings([]);
        return;
    }
    
    try {
        console.log("Loading bookings for user:", userId);
        
        // Call backend API
        const response = await apiRequest(API_ENDPOINTS.bookings, {
            method: 'GET'
        });
        
        if (response.status === 'success') {
            const bookings = response.data || [];
            console.log(`Loaded ${bookings.length} bookings`);
            
            // Sort by booking date (newest first)
            bookings.sort((a, b) => new Date(b.bookingDate) - new Date(a.bookingDate));
            
            // Display bookings
            displayUserBookings(bookings);
        } else {
            throw new Error(response.message || 'Failed to load bookings');
        }
    } catch (error) {
        console.error("Error loading bookings:", error);
        displayUserBookings([]);
    }
}
```

Also update the `updateUIForLoggedInUser()` function to call `loadUserBookings()`:

```javascript
function updateUIForLoggedInUser(user) {
    document.getElementById('auth-buttons').classList.add('hidden');
    document.getElementById('profile-menu').classList.remove('hidden');
    document.getElementById('sidebar-user-email').textContent = user.email || 'Welcome!';
    
    // Pre-fill profile page
    document.getElementById('profile-email').value = user.email || '';
    document.getElementById('profile-name').value = user.name || '';
    document.getElementById('profile-phone').value = user.phone || '';
    
    // Load user's bookings
    loadUserBookings();
}
```

---

### Step 10: Remove Unused Firebase Functions

Delete or comment out these functions that are no longer needed:
- `showAppInitializationError()` - Can keep but simplify
- Any Firebase-specific error handling

Keep the `displayUserBookings()` function as-is since it only handles UI rendering.

---

## API Integration Points

### Summary of All Changes

| Function | Before (Firebase) | After (C++ Backend) |
|----------|------------------|---------------------|
| `initializeAppCore()` | Initializes Firebase | Checks localStorage for token |
| `handleLogin()` | `signInWithEmailAndPassword()` | `POST /api/auth/login` |
| `handleRegister()` | `createUserWithEmailAndPassword()` | `POST /api/auth/register` |
| `handleLogout()` | `signOut()` | `POST /api/auth/logout` + clear localStorage |
| `handleProfileUpdate()` | Not implemented | `PUT /api/auth/profile` |
| `handleTrainSearch()` | Fetches from C++ or mock | `GET /api/search` |
| `handlePayment()` | Saves to Firestore | `POST /api/bookings` |
| `attachBookingsListener()` | Real-time Firestore listener | `GET /api/bookings` |

---

### Authentication Flow

```
┌─────────────┐         ┌──────────────┐         ┌──────────────┐
│  Frontend   │         │   Backend    │         │   Database   │
└──────┬──────┘         └──────┬───────┘         └──────┬───────┘
       │                       │                        │
       │  1. Login Request     │                        │
       │  (email, password)    │                        │
       ├──────────────────────>│                        │
       │                       │                        │
       │                       │  2. Verify Credentials │
       │                       ├───────────────────────>│
       │                       │                        │
       │                       │  3. User Data          │
       │                       │<───────────────────────┤
       │                       │                        │
       │                       │  4. Generate JWT       │
       │                       │                        │
       │  5. Response          │                        │
       │  (user, token)        │                        │
       │<──────────────────────┤                        │
       │                       │                        │
       │  6. Store Token       │                        │
       │  in localStorage      │                        │
       │                       │                        │
       │  7. Subsequent        │                        │
       │  Requests (+ token)   │                        │
       ├──────────────────────>│                        │
       │                       │                        │
       │                       │  8. Validate Token     │
       │                       │                        │
       │  9. Response          │                        │
       │<──────────────────────┤                        │
```

---

## Testing Integration

### Step-by-Step Testing

#### 1. **Test Backend Connectivity**

Open browser console (F12) and run:

```javascript
fetch('http://localhost:18080/api/search?from=New%20Delhi%20(NDLS)&to=Mumbai%20(CST)')
  .then(r => r.json())
  .then(data => console.log(data))
  .catch(err => console.error(err));
```

Expected output:
```json
{
  "status": "success",
  "count": 5,
  "trains": [...]
}
```

#### 2. **Test User Registration**

1. Click "Register" button
2. Fill in:
   - Name: Test User
   - Email: test@example.com
   - Password: Test123456
3. Click "Create Account"
4. Check console for:
   - `Registration successful`
   - Profile menu should appear
   - Auth buttons should hide

**Verify in Console:**
```javascript
console.log("Token:", localStorage.getItem('authToken'));
console.log("User:", localStorage.getItem('currentUser'));
```

#### 3. **Test User Login**

1. Logout (if logged in)
2. Click "Log In" button
3. Enter credentials
4. Click "Log In"
5. Verify:
   - Profile menu appears
   - Welcome message with email
   - "My Bookings" shows empty state

#### 4. **Test Train Search**

1. Enter:
   - From: New Delhi (NDLS)
   - To: Mumbai (CST)
   - Date: Tomorrow's date
2. Click "Search"
3. Verify:
   - Loading spinner appears
   - Train results displayed
   - Each train shows classes and prices
   - "Book Now" buttons work

#### 5. **Test Booking Flow**

1. From search results, click a class button (e.g., "3A")
2. Verify:
   - Redirected to Passenger Details page
   - Journey summary shows correct train
3. Add passenger:
   - Name: John Doe
   - Age: 25
   - Gender: Male
   - Berth: Lower
4. Click "Add Passenger"
5. Verify passenger appears in list
6. Click "Proceed to Payment"
7. Verify payment page shows correct summary
8. Click "Pay Now"
9. Verify:
   - Confirmation page appears
   - PNR displayed
   - Seat assignments shown

#### 6. **Test My Bookings Page**

1. Click profile icon → "My Bookings"
2. Verify:
   - Recent booking appears
   - All details correct (train, PNR, passengers, seats)

#### 7. **Test Profile Update**

1. Click profile icon → "My Profile"
2. Update:
   - Name: Test User Updated
   - Phone: +91 98765 43210
3. Click "Update Profile"
4. Verify success message
5. Check sidebar shows updated name

#### 8. **Test Logout**

1. Click "Logout" in sidebar
2. Verify:
   - Redirected to home page
   - Auth buttons appear
   - Profile menu hidden
3. Refresh page
4. Verify still logged out

---

### Common Testing Scenarios

#### Scenario 1: Token Expiration

1. Login successfully
2. Manually expire token in backend (or wait if configured)
3. Try to create booking
4. Should show "Please log in again" message

#### Scenario 2: Network Error

1. Stop C++ backend server
2. Try to search trains
3. Should fallback to mock data
4. Try to login
5. Should show clear error message

#### Scenario 3: Invalid Credentials

1. Try to login with wrong password
2. Should show "Invalid email or password"
3. Try to register with existing email
4. Should show "Email already exists"

---

## Troubleshooting

### Problem: CORS Errors

**Error in Console:**
```
Access to fetch at 'http://localhost:18080/api/auth/login' from origin 'http://localhost:5500' 
has been blocked by CORS policy
```

**Solution:**
1. Ensure C++ backend has CORS headers in ALL responses
2. Check preflight OPTIONS requests are handled
3. Verify `Access-Control-Allow-Origin` header is set

**Backend Code:**
```cpp
// Add to every response
response.headers["Access-Control-Allow-Origin"] = "*";
response.headers["Access-Control-Allow-Methods"] = "GET, POST, PUT, DELETE, OPTIONS";
response.headers["Access-Control-Allow-Headers"] = "Content-Type, Authorization";

// Handle preflight
if (request.method == "OPTIONS") {
    response.statusCode = 204;
    return response;
}
```

---

### Problem: 401 Unauthorized Errors

**Error:** "Invalid or expired token"

**Solutions:**
1. Check token is being sent: `console.log(authToken)`
2. Verify token format: `Bearer <token>`
3. Check token expiration in backend
4. Clear localStorage and login again:
   ```javascript
   localStorage.clear();
   location.reload();
   ```

---

### Problem: Trains Not Loading

**Error:** "No trains found" or using mock data

**Solutions:**
1. Verify backend is running: `curl http://localhost:18080/api/search?from=X&to=Y`
2. Check database has train data
3. Verify station names match exactly (including codes)
4. Check backend logs for errors

---

### Problem: Bookings Not Saving

**Error:** "Error saving booking" or validation errors

**Solutions:**
1. Check you're logged in: `console.log(authToken, userId)`
2. Verify booking data format matches backend expectations
3. Check backend validation rules (passenger count, date range)
4. Verify sufficient seats available
5. Check backend logs for specific error

---

### Problem: Profile Not Loading After Refresh

**Issue:** User logged out after page refresh

**Solutions:**
1. Check `loadAuthToken()` and `loadUserData()` are called in `initializeAppCore()`
2. Verify localStorage has data:
   ```javascript
   console.log(localStorage.getItem('authToken'));
   console.log(localStorage.getItem('currentUser'));
   ```
3. Check token validation call in `initializeAppCore()` succeeds
4. Verify token hasn't expired

---

### Problem: JSON Parsing Errors

**Error:** "Unexpected token < in JSON"

**Cause:** Backend returned HTML error page instead of JSON

**Solutions:**
1. Check backend endpoint exists and is correct
2. Verify backend returns JSON even for errors
3. Check backend error handling
4. Look at actual response: `console.log(response.text())`

---

## Production Deployment

### Frontend Deployment

#### Option 1: Static Hosting (Netlify, Vercel, GitHub Pages)

1. **Update API Base URL** in `index.html`:
   ```javascript
   const API_BASE_URL = "https://your-backend-domain.com/api";
   ```

2. **Deploy HTML file**:
   ```bash
   # For Netlify
   netlify deploy --prod --dir=.
   
   # For Vercel
   vercel --prod
   
   # For GitHub Pages
   git add index.html
   git commit -m "Deploy"
   git push origin main
   ```

#### Option 2: Traditional Web Server (Apache, Nginx)

1. Copy `index.html` to web root:
   ```bash
   sudo cp index.html /var/www/html/
   ```

2. Configure server for SPA (if needed)

---

### Backend Deployment

#### 1. **Environment Variables**

Create `.env` file:
```bash
PORT=18080
DB_PATH=/var/lib/traintrack/database.db
JWT_SECRET=your-production-secret-key-change-this
CORS_ORIGIN=https://your-frontend-domain.com
```

#### 2. **Compile for Production**

```bash
g++ -O3 -std=c++17 -o traintrack_server main.cpp -lsqlite3 -lpthread
```

#### 3. **Run as Service (Linux)**

Create `/etc/systemd/system/traintrack.service`:
```ini
[Unit]
Description=TrainTrack Backend Server
After=network.target

[Service]
Type=simple
User=www-data
WorkingDirectory=/opt/traintrack
ExecStart=/opt/traintrack/traintrack_server
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

Enable and start:
```bash
sudo systemctl enable traintrack
sudo systemctl start traintrack
sudo systemctl status traintrack
```

#### 4. **Reverse Proxy (Nginx)**

Create `/etc/nginx/sites-available/traintrack`:
```nginx
server {
    listen 80;
    server_name api.traintrack.com;
    
    location /api {
        proxy_pass http://localhost:18080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```

Enable and restart:
```bash
sudo ln -s /etc/nginx/sites-available/traintrack /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl restart nginx
```

#### 5. **SSL Certificate (Let's Encrypt)**

```bash
sudo apt install certbot python3-certbot-nginx
sudo certbot --nginx -d api.traintrack.com
```

---

### Security Checklist

- [ ] Change JWT secret from default
- [ ] Enable HTTPS (SSL certificate)
- [ ] Restrict CORS to frontend domain only
- [ ] Use environment variables for secrets
- [ ] Implement rate limiting
- [ ] Enable database backups
- [ ] Set up logging and monitoring
- [ ] Use strong password hashing (bcrypt)
- [ ] Validate all user inputs
- [ ] Sanitize SQL queries (use parameterized queries)

---

### Monitoring & Maintenance

#### 1. **Health Check Endpoint**

Add to backend:
```cpp
// GET /api/health
Response handleHealth(const Request& request) {
    Response response(200);
    response.setJson({
        {"status", "ok"},
        {"timestamp", getCurrentTimestamp()},
        {"version", "1.0.0"}
    });
    return response;
}
```

#### 2. **Log Files**

Monitor these logs:
- Application logs: `/var/log/traintrack/app.log`
- Nginx access: `/var/log/nginx/access.log`
- Nginx errors: `/var/log/nginx/error.log`
- System logs: `journalctl -u traintrack`

#### 3. **Database Backups**

Setup daily backups:
```bash
#!/bin/bash
# /opt/traintrack/backup.sh
BACKUP_DIR="/var/backups/traintrack"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)

mkdir -p $BACKUP_DIR
sqlite3 /var/lib/traintrack/database.db ".backup $BACKUP_DIR/db_$TIMESTAMP.db"

# Keep only last 7 days
find $BACKUP_DIR -name "db_*.db" -mtime +7 -delete
```

Add to crontab:
```bash
0 2 * * * /opt/traintrack/backup.sh
```

---

## Summary

### Integration Checklist

- [ ] Backend server running on port 18080
- [ ] CORS enabled in backend
- [ ] Database initialized with train data
- [ ] Frontend `index.html` updated with all changes:
  - [ ] Removed Firebase imports
  - [ ] Added API helper functions
  - [ ] Updated configuration
  - [ ] Modified authentication functions
  - [ ] Updated train search
  - [ ] Modified booking functions
  - [ ] Changed bookings display
- [ ] Tested all features:
  - [ ] Registration
  - [ ] Login
  - [ ] Train search
  - [ ] Booking creation
  - [ ] View bookings
  - [ ] Profile update
  - [ ] Logout
- [ ] Production deployment configured
- [ ] Monitoring and backups set up

### Key Files Modified

1. **index.html** - Main frontend file with all changes
2. **Backend API** - All endpoints implemented
3. **Database** - Schema created and seeded

### Support & Resources

- Backend Requirements: See `BACKEND_REQUIREMENTS.md`
- API Documentation: See endpoint descriptions in this guide
- Frontend Code: All changes documented above

---

**Congratulations!** Your TrainTrack application should now be fully integrated with the C++ backend. Users can register, login, search trains, make bookings, and manage their profile - all powered by your custom C++ server!

For questions or issues, refer to the Troubleshooting section or check backend logs for detailed error messages.
