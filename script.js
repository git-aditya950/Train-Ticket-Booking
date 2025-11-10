// ==================================
//  1. FIREBASE IMPORTS
// ==================================
// All imports must be at the top of a module
import { initializeApp } from "https://www.gstatic.com/firebasejs/10.12.2/firebase-app.js";
import { 
    getAuth, 
    onAuthStateChanged, 
    createUserWithEmailAndPassword, 
    signInWithEmailAndPassword, 
    signOut,
    signInAnonymously,
    signInWithCustomToken,
    setPersistence,
    inMemoryPersistence
} from "https://www.gstatic.com/firebasejs/10.12.2/firebase-auth.js";
import { 
    getFirestore, 
    doc, 
    setDoc, 
    addDoc, 
    collection, 
    onSnapshot, 
    query,
    Timestamp,
    setLogLevel
} from "https://www.gstatic.com/firebasejs/10.12.2/firebase-firestore.js";

// ==================================
//  2. FIREBASE CONFIG
// ==================================
const firebaseConfig = JSON.parse(typeof __firebase_config !== 'undefined' ? __firebase_config : '{}');
const appId = typeof __app_id !== 'undefined' ? __app_id : 'default-app-id';

if (!firebaseConfig.apiKey) {
    console.warn("Firebase config is missing. App will not connect to database.");
}

// ==================================
//  3. FIREBASE SETUP FUNCTION
// ==================================
async function setupFirebase() {
    try {
        // Initialize Firebase
        const app = initializeApp(firebaseConfig);
        const auth = getAuth(app);
        const db = getFirestore(app);
        setLogLevel('debug');

        // Attach Firebase functions to the window object so app logic can use them
        window.firebase = {
            auth,
            db,
            onAuthStateChanged,
            createUserWithEmailAndPassword,
            signInWithEmailAndPassword,
            signOut,
            signInAnonymously,
            signInWithCustomToken,
            doc,
            setDoc,
            addDoc,
            collection,
            onSnapshot,
            query,
            Timestamp,
            appId
        };

        // Handle Initial Authentication
        await setPersistence(auth, inMemoryPersistence);
        if (typeof __initial_auth_token !== 'undefined' && __initial_auth_token) {
            console.log("Signing in with custom token...");
            await signInWithCustomToken(auth, __initial_auth_token);
        } else {
            console.log("No auth token found, signing in anonymously...");
            await signInAnonymously(auth);
        }
        return true; // Signal success
    } catch (e) {
        console.error("Error loading or authenticating Firebase modules. App may not function correctly.", e);
        return false; // Signal failure
    }
}

// ==================================
//  4. APPLICATION LOGIC
// ==================================
function initializeAppLogic() {
    console.log("Firebase is ready, initializing app logic...");
                
    // --- Get Firebase functions from the window ---
    const {
        auth, db, onAuthStateChanged,
        createUserWithEmailAndPassword, signInWithEmailAndPassword, signOut,
        doc, setDoc, addDoc, collection, onSnapshot, query, Timestamp, appId
    } = window.firebase;

    // --- Global App State ---
    let currentUserId = null;
    let currentBooking = { train: null, passengers: [] };
    let bookingsListener = null; // To unsubscribe from snapshot

    // --- Page Navigation ---
    const navLinks = document.querySelectorAll('.nav-link');
    const pages = document.querySelectorAll('.page-content');
    const logoHomeLink = document.getElementById('logo-home-link');
    
    // --- Search Form & Results ---
    const searchForm = document.getElementById('search-form');
    const searchResults = document.getElementById('search-results');
    const journeyDateInput = document.getElementById('journey-date');
    const resultsContainer = document.getElementById('search-results-container');
    const resultsBackBtn = document.getElementById('results-back-btn');
    const API_URL = 'http://localhost:18080/api/search';

    // --- Modal Elements ---
    const modalOverlay = document.getElementById('modal-overlay');
    const loginBtn = document.getElementById('login-btn');
    const registerBtn = document.getElementById('register-btn');
    const loginModal = document.getElementById('login-modal');
    const registerModal = document.getElementById('register-modal');
    const closeLoginModal = document.getElementById('close-login-modal');
    const closeRegisterModal = document.getElementById('close-register-modal');
    const showRegisterFromLogin = document.getElementById('show-register-from-login');

    // --- Auth State Elements ---
    const authButtons = document.getElementById('auth-buttons');
    const loginForm = document.getElementById('login-form');
    const loginEmailInput = document.getElementById('login-email');
    const loginPasswordInput = document.getElementById('login-password'); // Get password input
    const registerForm = document.getElementById('register-form');
    const registerEmailInput = document.getElementById('register-email');
    const registerPasswordInput = document.getElementById('register-password'); // Get password input

    // --- SIDEBAR ELEMENTS ---
    const profileMenu = document.getElementById('profile-menu');
    const profileBadgeBtn = document.getElementById('profile-badge-btn');
    const sidebarMenu = document.getElementById('sidebar-menu');
    const closeSidebarBtn = document.getElementById('close-sidebar-btn');
    const sidebarUserEmail = document.getElementById('sidebar-user-email');
    const sidebarLogoutBtn = document.getElementById('sidebar-logout-btn');
    const sidebarLinkProfile = document.getElementById('sidebar-link-profile');
    const sidebarLinkBookings = document.getElementById('sidebar-link-bookings');
    const profileViewBookings = document.getElementById('profile-view-bookings');
    
    // --- GEMINI API ELEMENTS ---
    const GEMINI_API_KEY = ""; // Leave blank
    const GEMINI_API_URL_FLASH = `https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash-preview-09-2025:generateContent?key=${GEMINI_API_KEY}`;
    const getInspirationBtn = document.getElementById('get-inspiration-btn');
    const inspirationPrompt = document.getElementById('inspiration-prompt');
    const inspirationResults = document.getElementById('inspiration-results');
    const geminiModal = document.getElementById('gemini-modal');
    const closeGeminiModal = document.getElementById('close-gemini-modal');
    const geminiModalTitle = document.getElementById('gemini-modal-title');
    const geminiLoading = document.getElementById('gemini-loading');
    const geminiContent = document.getElementById('gemini-content');

    // --- BOOKING FLOW ELEMENTS ---
    const addPassengerForm = document.getElementById('add-passenger-form');
    const passengerListContainer = document.getElementById('passenger-list-container');
    const passengerJourneySummary = document.getElementById('passenger-journey-summary');
    const passengerBackBtn = document.getElementById('passenger-back-btn');
    const passengerProceedBtn = document.getElementById('passenger-proceed-btn');

    const paymentForm = document.getElementById('payment-form');
    const paymentJourneySummary = document.getElementById('payment-journey-summary');
    const paymentPassengerSummary = document.getElementById('payment-passenger-summary');
    const paymentTotalFare = document.getElementById('payment-total-fare');

    const confirmationPNR = document.getElementById('confirmation-pnr');
    const confirmationJourneySummary = document.getElementById('confirmation-journey-summary');
    const confirmationPassengerSummary = document.getElementById('confirmation-passenger-summary');
    const confirmationHomeBtn = document.getElementById('confirmation-home-btn');

    // --- "MY BOOKINGS" ELEMENTS ---
    const bookingsContainer = document.getElementById('bookings-container');
    const noBookingsMessage = document.getElementById('no-bookings-message');

    // --- MASTER TRAIN LIST (unchanged) ---
    const ALL_TRAINS = [
        { trainNumber: "12001", trainName: "Shatabdi Express", fare: 1250 },
        { trainNumber: "12953", trainName: "August Kranti Rajdhani", fare: 2450 },
        { trainNumber: "22439", trainName: "Vande Bharat Express", fare: 1800 },
        { trainNumber: "12301", trainName: "Rajdhani Express", fare: 2300 },
        { trainNumber: "12049", trainName: "Gatimaan Express", fare: 1500 },
        { trainNumber: "12259", trainName: "Duronto Express", fare: 2100 },
        { trainNumber: "12137", trainName: "Punjab Mail", fare: 900 },
        { trainNumber: "12615", trainName: "Grand Trunk Express", fare: 1100 },
        { trainNumber: "12859", trainName: "Gitanjali Express", fare: 1050 },
        { trainNumber: "11077", trainName: "Jhelum Express", fare: 950 },
        { trainNumber: "12419", trainName: "Gomti Express", fare: 700 },
        { trainNumber: "12559", trainName: "Shiv Ganga Express", fare: 1300 },
        { trainNumber: "12723", trainName: "Telangana Express", fare: 1150 },
        { trainNumber: "22691", trainName: "KSR Bengaluru Rajdhani", fare: 2500 },
        { trainNumber: "12451", trainName: "Shram Shakti Express", fare: 850 },
        { trainNumber: "12903", trainName: "Golden Temple Mail", fare: 1000 },
        { trainNumber: "12393", trainName: "Sampoorna Kranti", fare: 1200 },
        { trainNumber: "12321", trainName: "HWH-Mumbai Mail", fare: 980 },
        { trainNumber: "12190", trainName: "Mahakoshal Express", fare: 920 },
        { trainNumber: "12715", trainName: "Sachkhand Express", fare: 1100 }
    ];
    function getRandomTrains(from, to) {
        let shuffled = [...ALL_TRAINS].sort(() => 0.5 - Math.random());
        let selectedCount = 3 + Math.floor(Math.random() * 3);
        let selectedTrains = shuffled.slice(0, selectedCount);
        const times = ["06:00", "08:15", "11:30", "14:05", "17:15", "20:00", "22:45"];
        const durations = ["6h 00m", "8h 15m", "10h 30m", "12h 45m", "15h 00m"];
        const statuses = [
            { status: "AVL 120", color: "green" }, { status: "AVL 45", color: "green" },
            { status: "RAC 15", color: "orange" }, { status: "WL 30", color: "orange" },
            { status: "NOT AVAILABLE", color: "red" },
        ];
        return selectedTrains.map(train => {
            const depTime = times[Math.floor(Math.random() * times.length)];
            const arrTime = times[Math.floor(Math.random() * times.length)];
            const duration = durations[Math.floor(Math.random() * durations.length)];
            return {
                ...train, runsOn: "M, Tu, W, Th, F, Sa, Su".substring(0, Math.floor(5 + Math.random() * 20)),
                departureTime: depTime, departureStation: from,
                arrivalTime: arrTime, arrivalStation: to, duration: duration,
                availability: [
                    {"class": "SL", ...statuses[Math.floor(Math.random() * statuses.length)]},
                    {"class": "3A", ...statuses[Math.floor(Math.random() * statuses.length)]},
                    {"class": "2A", ...statuses[Math.floor(Math.random() * statuses.length)]}
                ]
            };
        });
    }
    // --- End of Train List ---


    // --- GEMINI API CALL FUNCTION (unchanged) ---
    async function callGeminiAPI(prompt, useGrounding = false) {
        const payload = {
            contents: [{ parts: [{ text: prompt }] }],
            systemInstruction: {
                parts: [{ text: "You are a helpful travel assistant for an Indian Railways booking website. Be concise and friendly." }]
            },
        };
        if (useGrounding) payload.tools = [{ "google_search": {} }];

        let response;
        let retries = 3;
        let delay = 1000; 

        while (retries > 0) {
            try {
                response = await fetch(GEMINI_API_URL_FLASH, {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify(payload)
                });
                if (response.status === 429) throw new Error('Throttled');
                if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
                break; 
            } catch (error) {
                retries--;
                if (retries === 0) return { text: "Sorry, I'm having trouble connecting to the AI. Please try again later.", sources: [] };
                await new Promise(resolve => setTimeout(resolve, delay));
                delay *= 2;
            }
        }
        try {
            const result = await response.json();
            const candidate = result.candidates?.[0];
            if (candidate && candidate.content?.parts?.[0]?.text) {
                const text = candidate.content.parts[0].text;
                let sources = [];
                const groundingMetadata = candidate.groundingMetadata;
                if (groundingMetadata && groundingMetadata.groundingAttributions) {
                    sources = groundingMetadata.groundingAttributions
                        .map(attribution => ({ uri: attribution.web?.uri, title: attribution.web?.title }))
                        .filter(source => source.uri && source.title);
                }
                return { text, sources };
            } else {
                return { text: "Sorry, I couldn't get a valid response from the AI.", sources: [] };
            }
        } catch (error) {
            return { text: "Sorry, there was an error processing the AI response.", sources: [] };
        }
    }
    // --- END GEMINI API CALL FUNCTION ---


    // --- GEMINI FEATURE 1 Logic (unchanged) ---
    getInspirationBtn.addEventListener('click', async () => {
        const prompt = inspirationPrompt.value;
        if (!prompt) {
            inspirationResults.innerHTML = `<p class="text-red-600">Please enter a description of your ideal trip.</p>`;
            return;
        }
        inspirationResults.innerHTML = `<div class="text-center text-gray-600"><p class="font-medium">Searching for destinations...</p></div>`;
        const fullPrompt = `Suggest 5 train-accessible travel destinations in India for '${prompt}'. Format as an HTML unordered list. For each destination, include the main railway station.`;
        const { text, sources } = await callGeminiAPI(fullPrompt, true);
        let sourcesHtml = '';
        if (sources.length > 0) {
            sourcesHtml = '<h4 class="font-semibold mt-4">Learn more (from Google Search):</h4><ul class="list-disc pl-5 text-sm">';
            sources.forEach(source => {
                sourcesHtml += `<li><a href="${source.uri}" target="_blank" rel="noopener noreferrer" class="text-indigo-600 hover:underline">${source.title}</a></li>`;
            });
            sourcesHtml += '</ul>';
        }
        inspirationResults.innerHTML = `<div class="mt-4 p-4 bg-indigo-50 border border-indigo-200 rounded-lg">${text} ${sourcesHtml}</div>`;
    });

    // --- GEMINI FEATURE 2 Logic (unchanged) ---
    async function showTripPlanner(destination) {
        geminiModalTitle.textContent = `✨ Trip Ideas for ${destination}`;
        geminiLoading.classList.remove('hidden');
        geminiContent.innerHTML = '';
        geminiContent.classList.add('hidden');
        showGeminiModal();
        const fullPrompt = `Act as a tour guide. Provide a 3-point mini-itinerary for a tourist arriving at '${destination}'. Focus on attractions near the station. Make it exciting and concise, formatted as an HTML unordered list.`;
        const { text } = await callGeminiAPI(fullPrompt, false);
        geminiLoading.classList.add('hidden');
        geminiContent.innerHTML = text; 
        geminiContent.classList.remove('hidden');
    }
    
    
    // --- MODAL & SIDEBAR Logic (unchanged) ---
    function showLoginModal() {
        modalOverlay.classList.remove('hidden');
        loginModal.classList.remove('hidden');
        registerModal.classList.add('hidden');
    }
    function hideLoginModal() {
        modalOverlay.classList.add('hidden');
        loginModal.classList.add('hidden');
    }
    function showRegisterModal() {
        modalOverlay.classList.remove('hidden');
        registerModal.classList.remove('hidden');
        loginModal.classList.add('hidden'); 
    }
    function hideRegisterModal() {
        modalOverlay.classList.add('hidden');
        registerModal.classList.add('hidden');
    }
    function showSidebar() {
        modalOverlay.classList.remove('hidden');
        sidebarMenu.classList.remove('translate-x-full');
    }
    function hideSidebar() {
        modalOverlay.classList.add('hidden');
        sidebarMenu.classList.add('translate-x-full');
    }
    function showGeminiModal() {
        modalOverlay.classList.remove('hidden');
        geminiModal.classList.remove('hidden');
    }
    function hideGeminiModal() {
        modalOverlay.classList.add('hidden');
        geminiModal.classList.add('hidden');
        geminiContent.innerHTML = '';
    }
    
    loginBtn.addEventListener('click', (e) => { e.preventDefault(); showLoginModal(); });
    registerBtn.addEventListener('click', (e) => { e.preventDefault(); showRegisterModal(); });
    profileBadgeBtn.addEventListener('click', (e) => { e.preventDefault(); showSidebar(); });
    closeLoginModal.addEventListener('click', (e) => { e.preventDefault(); hideLoginModal(); });
    closeRegisterModal.addEventListener('click', (e) => { e.preventDefault(); hideRegisterModal(); });
    closeSidebarBtn.addEventListener('click', (e) => { e.preventDefault(); hideSidebar(); });
    closeGeminiModal.addEventListener('click', (e) => { e.preventDefault(); hideGeminiModal(); });
    showRegisterFromLogin.addEventListener('click', (e) => { e.preventDefault(); showRegisterModal(); });
    modalOverlay.addEventListener('click', () => {
        hideLoginModal();
        hideRegisterModal();
        hideGeminiModal();
        hideSidebar();
    });


    // --- REAL AUTHENTICATION LOGIC ---
    onAuthStateChanged(auth, (user) => {
        if (user && !user.isAnonymous) {
            // User is signed in
            console.log("User logged in:", user.email, user.uid);
            currentUserId = user.uid;
            sidebarUserEmail.textContent = user.email || 'User';
            authButtons.classList.add('hidden');
            profileMenu.classList.remove('hidden');
            
            // Load this user's bookings
            loadUserBookings(currentUserId);

        } else {
            // User is signed out or anonymous
            console.log("User logged out or anonymous.");
            currentUserId = null;
            sidebarUserEmail.textContent = 'demo@user.com'; // Reset
            authButtons.classList.remove('hidden');
            profileMenu.classList.add('hidden');

            if (bookingsListener) {
                bookingsListener(); // This is the unsubscribe function
                bookingsListener = null;
            }
            renderBookings([]);
        }
        hideLoginModal();
        hideRegisterModal();
        showPage('page-search');
    });

    registerForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        const email = registerEmailInput.value;
        const password = registerPasswordInput.value;
        try {
            const userCredential = await createUserWithEmailAndPassword(auth, email, password);
            console.log("User registered:", userCredential.user);
        } catch (error) {
            console.error("Registration Error:", error);
            alert(`Registration failed: ${error.message}`);
        }
    });

    loginForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        const email = loginEmailInput.value;
        const password = loginPasswordInput.value;
        try {
            const userCredential = await signInWithEmailAndPassword(auth, email, password);
            console.log("User logged in:", userCredential.user);
        } catch (error) {
            console.error("Login Error:", error);
            alert(`Login failed: ${error.message}`);
        }
    });

    sidebarLogoutBtn.addEventListener('click', async (e) => {
        e.preventDefault();
        try {
            await signOut(auth);
        } catch (error) {
            console.error("Logout Error:", error);
        }
        hideSidebar();
    });

    // --- "MY BOOKINGS" DATABASE LOGIC ---
    function loadUserBookings(userId) {
        console.log(`Setting up bookings listener for user: ${userId}`);
        const bookingsPath = `artifacts/${appId}/users/${userId}/bookings`;
        const q = query(collection(db, bookingsPath));

        if (bookingsListener) bookingsListener();

        bookingsListener = onSnapshot(q, (querySnapshot) => {
            const bookings = [];
            querySnapshot.forEach((doc) => {
                bookings.push({ id: doc.id, ...doc.data() });
            });
            console.log("Fetched bookings:", bookings);
            renderBookings(bookings);
        }, (error) => {
            console.error("Error fetching bookings: ", error);
        });
    }

    function renderBookings(bookings) {
        bookingsContainer.innerHTML = '';
        
        if (bookings.length === 0) {
            bookingsContainer.appendChild(noBookingsMessage); 
            return;
        }

        bookings.reverse(); 

        bookings.forEach(booking => {
            const train = booking.train;
            const passengers = booking.passengers;
            const pnr = booking.pnr;

            if (!train || !passengers || !pnr) {
                console.warn("Skipping invalid booking object:", booking);
                return;
            }

            const isUpcoming = !booking.isPast; 
            const passengerHtml = passengers.map(p => `<strong>${p.name}</strong> (${p.age}, ${p.gender})`).join(', ');

            const bookingCardHtml = `
            <div class="bg-gray-50 border border-gray-200 rounded-lg p-6 ${!isUpcoming ? 'opacity-70' : ''}">
                <div class="flex justify-between items-center">
                    <h3 class="text-lg font-bold ${isUpcoming ? 'text-indigo-700' : 'text-gray-700'}">${train.trainNumber} - ${train.trainName}</h3>
                    <span class="px-3 py-1 text-sm font-medium ${isUpcoming ? 'text-green-800 bg-green-100' : 'text-gray-800 bg-gray-200'} rounded-full">
                        ${isUpcoming ? 'Confirmed' : 'Completed'}
                    </span>
                </div>
                <p class="text-sm text-gray-500">PNR: <span class="font-medium text-gray-700">${pnr}</span></p>
                <div class="flex items-center space-x-8 mt-4">
                    <div>
                        <div class="text-lg font-semibold">${train.departureTime}</div>
                        <div class="text-sm text-gray-500">${train.departureStation}</div>
                    </div>
                    <div class="text-gray-400">&mdash; ${train.duration} &mdash;</div>
                    <div>
                        <div class="text-lg font-semibold">${train.arrivalTime}</div>
                        <div class="text-sm text-gray-500">${train.arrivalStation}</div>
                    </div>
                </div>
                <div class="mt-4 pt-4 border-t border-gray-200">
                    <p class="text-sm text-gray-600">Passengers: ${passengerHtml}</p>
                </div>
            </div>
            `;
            bookingsContainer.innerHTML += bookingCardHtml;
        });
    }

    // --- Sidebar & Profile navigation (unchanged) ---
    sidebarLinkProfile.addEventListener('click', (e) => { e.preventDefault(); showPage('page-profile'); hideSidebar(); });
    sidebarLinkBookings.addEventListener('click', (e) => { e.preventDefault(); showPage('page-bookings'); hideSidebar(); });
    profileViewBookings.addEventListener('click', (e) => { e.preventDefault(); showPage('page-bookings'); });

    // --- 60-Day Booking Window Logic (unchanged) ---
    function formatDate(date) { return date.toISOString().split('T')[0]; }
    const today = new Date();
    const maxDate = new Date();
    maxDate.setDate(today.getDate() + 60);
    const todayString = formatDate(today);
    const maxDateString = formatDate(maxDate);
    journeyDateInput.value = todayString; 
    journeyDateInput.min = todayString;   
    journeyDateInput.max = maxDateString; 
    
    // --- Page Navigation Logic (unchanged) ---
    function showPage(pageId) {
        pages.forEach(page => page.classList.add('hidden'));
        const pageElement = document.getElementById(pageId);
        if (pageElement) {
            pageElement.classList.remove('hidden');
        } else {
            console.error(`Page with ID '${pageId}' not found.`);
        }
        
        window.scrollTo(0, 0); 
        navLinks.forEach(link => {
            link.classList.remove('border-indigo-500', 'text-gray-900');
            link.classList.add('border-transparent', 'text-gray-500');
        });
        if (pageId === 'page-search' || pageId === 'page-search-results' || pageId.startsWith('page-passenger') || pageId.startsWith('page-payment') || pageId.startsWith('page-confirmation')) {
            const activeLink = document.getElementById('nav-search');
            if (activeLink) {
                activeLink.classList.add('border-indigo-500', 'text-gray-900');
                activeLink.classList.remove('border-transparent', 'text-gray-500');
            }
        }
    }
    document.getElementById('nav-search').addEventListener('click', (e) => { e.preventDefault(); showPage('page-search'); });
    logoHomeLink.addEventListener('click', (e) => { e.preventDefault(); showPage('page-search'); });
    resultsBackBtn.addEventListener('click', (e) => { e.preventDefault(); showPage('page-search'); });
    showPage('page-search'); 

    // --- Search Form Handling (unchanged) ---
    searchForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        const from = document.getElementById('from-station').value || "New Delhi (NDLS)";
        const to = document.getElementById('to-station').value || "Mumbai (CST)";
        if (!from || !to) {
           resultsContainer.innerHTML = `<div class="bg-red-100 border border-red-400 text-red-700 px-4 py-3 rounded-lg" role="alert">
               Please fill in both 'From' and 'To' stations.
           </div>`;
           showPage('page-search-results'); 
           return;
        }
        resultsContainer.innerHTML = `<div class="text-center text-gray-600">
            <p class="text-lg font-medium">Searching for trains...</p>
        </div>`;
        showPage('page-search-results'); 
        try {
            const randomTrains = getRandomTrains(from, to);
            const mockData = { "status": "success", "trains": randomTrains };
            await new Promise(resolve => setTimeout(resolve, 1000));
            const data = mockData; 
            
            if (data.status === 'success' && data.trains) {
                renderTrainResults(data.trains);
            } else {
                throw new Error('Invalid data format from server.');
            }
        } catch (error) {
            console.error('Fetch error:', error);
            resultsContainer.innerHTML = `<div class="bg-red-100 border border-red-400 text-red-700 px-4 py-3 rounded-lg" role="alert">
                <strong>Error:</strong> Could not fetch train data.
                <p class="mt-2 text-sm">Details: ${error.message}</p>
            </div>`;
        }
    });

    // --- Render Train Results Function (unchanged) ---
    function renderTrainResults(trains) {
        resultsContainer.innerHTML = '';
        if (trains.length === 0) {
            resultsContainer.innerHTML = `<div class="bg-blue-100 border border-blue-400 text-blue-700 px-4 py-3 rounded-lg" role="alert">
                No trains found for this route.
            </div>`;
            return;
        }
        trains.forEach(train => {
            const availabilityHtml = train.availability.map(avail => `
                <div class="text-center">
                    <div class="font-medium ${
                        avail.color === 'green' ? 'text-green-600' :
                        (avail.color === 'orange' ? 'text-orange-600' : 'text-red-600')
                    }">${avail.status}</div>
                    <div class="text-sm text-gray-500">${avail.class}</div>
                </div>
            `).join('');
            const trainCardHtml = `
            <div class="bg-white p-6 rounded-2xl shadow-lg border border-gray-200 mb-6 train-card" 
                 data-train='${JSON.stringify(train)}'>
                <div class="flex flex-col md:flex-row justify-between md:items-center">
                    <div>
                        <h3 class="text-xl font-bold text-indigo-700">${train.trainNumber} - ${train.trainName}</h3>
                        <p class="text-gray-600">Runs On: ${train.runsOn}</p>
                        <div class="flex items-center space-x-8 mt-2">
                            <div>
                                <div class="text-lg font-semibold">${train.departureTime}</div>
                                <div class="text-sm text-gray-500">${train.departureStation}</div>
                            </div>
                            <div class="text-gray-400">&mdash; ${train.duration} &mdash;</div>
                            <div>
                                <div class="text-lg font-semibold">${train.arrivalTime}</div>
                                <div class="text-sm text-gray-500">${train.arrivalStation}</div>
                            </div>
                        </div>
                    </div>
                    <div class="mt-4 md:mt-0 md:text-right">
                        <div class="flex md:justify-end space-x-4 mb-2">
                            ${availabilityHtml}
                        </div>
                        <div class="flex flex-col sm:flex-row sm:space-x-2">
                            <button class="mt-2 px-6 py-2 text-sm font-medium text-white bg-indigo-600 rounded-lg shadow-sm hover:bg-indigo-700 transition book-now-btn">
                                Book Now
                            </button>
                            <button class="mt-2 px-4 py-2 text-sm font-medium text-indigo-700 bg-indigo-100 rounded-lg shadow-sm hover:bg-indigo-200 transition gemini-plan-btn" data-destination="${train.arrivalStation}">
                                ✨ Plan My Trip
                            </button>
                        </div>
                    </div>
                </div>
            </div>
            `;
            resultsContainer.innerHTML += trainCardHtml;
        });
    }

    // --- BOOKING FLOW LOGIC (Part 1 - Click) (unchanged) ---
    resultsContainer.addEventListener('click', (e) => {
        const trainCard = e.target.closest('.train-card');
        if (!trainCard) return;
        const trainData = JSON.parse(trainCard.dataset.train);
        
        if (e.target.classList.contains('book-now-btn')) {
            e.preventDefault();
            if (!currentUserId) {
                showLoginModal();
            } else {
                startBooking(trainData);
            }
        }
        
        if (e.target.classList.contains('gemini-plan-btn')) {
            e.preventDefault();
            showTripPlanner(trainData.arrivalStation);
        }
    });

    // --- BOOKING FLOW LOGIC (Part 2 - Passenger) (unchanged) ---
    function startBooking(trainData) {
        currentBooking.train = trainData;
        currentBooking.passengers = [];
        updatePassengerPage();
        showPage('page-passenger-details');
    }
    function getJourneySummaryHtml(train) {
        return `
            <h3 class="text-lg font-bold text-indigo-700">${train.trainNumber} - ${train.trainName}</h3>
            <div class="flex items-center space-x-4 mt-2">
                <div>
                    <div class="text-md font-semibold">${train.departureTime}</div>
                    <div class="text-sm text-gray-500">${train.departureStation}</div>
                </div>
                <div class="text-gray-400">&mdash; ${train.duration} &mdash;</div>
                <div>
                    <div class="text-md font-semibold">${train.arrivalTime}</div>
                    <div class="text-sm text-gray-500">${train.arrivalStation}</div>
                </div>
            </div>
        `;
    }
    function updatePassengerPage() {
        passengerJourneySummary.innerHTML = getJourneySummaryHtml(currentBooking.train);
        renderPassengerList();
        addPassengerForm.reset();
        passengerProceedBtn.disabled = currentBooking.passengers.length === 0;
    }
    function renderPassengerList() {
        passengerListContainer.innerHTML = '';
        if (currentBooking.passengers.length === 0) {
            passengerListContainer.innerHTML = `<p class="text-gray-500 text-sm">No passengers added yet.</p>`;
        }
        currentBooking.passengers.forEach((p, index) => {
            passengerListContainer.innerHTML += `
                <div class="flex justify-between items-center bg-white p-3 rounded-lg border border-gray-200">
                    <div>
                        <p class="font-medium text-gray-900">${p.name}</p>
                        <p class="text-sm text-gray-500">${p.age}, ${p.gender} (Pref: ${p.berth})</p>
                    </div>
                    <button class="p-1 text-red-500 hover:text-red-700 remove-passenger-btn" data-index="${index}">
                        <svg class="w-5 h-5" xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="1.5" stroke="currentColor"><path stroke-linecap="round" stroke-linejoin="round" d="M6 18L18 6M6 6l12 12" /></svg>
                    </button>
                </div>
            `;
        });
    }
    addPassengerForm.addEventListener('submit', (e) => {
        e.preventDefault();
        currentBooking.passengers.push({
            name: document.getElementById('passenger-name').value,
            age: document.getElementById('passenger-age').value,
            gender: document.getElementById('passenger-gender').value,
            berth: document.getElementById('passenger-berth').value,
        });
        updatePassengerPage();
    });
    passengerListContainer.addEventListener('click', (e) => {
        const removeBtn = e.target.closest('.remove-passenger-btn');
        if (removeBtn) {
            currentBooking.passengers.splice(removeBtn.dataset.index, 1);
            updatePassengerPage();
        }
    });
    passengerBackBtn.addEventListener('click', (e) => { e.preventDefault(); showPage('page-search-results'); });
    passengerProceedBtn.addEventListener('click', (e) => {
        e.preventDefault();
        startPayment();
    });

    // --- BOOKING FLOW LOGIC (Part 3 - Payment) (unchanged) ---
    function startPayment() {
        const train = currentBooking.train;
        const passengers = currentBooking.passengers;
        const totalFare = (train.fare || 1000) * passengers.length;
        paymentJourneySummary.innerHTML = getJourneySummaryHtml(train);
        paymentPassengerSummary.innerHTML = `
            <p class="text-sm font-medium text-gray-700">Passengers (${passengers.length}):</p>
            <p class="text-sm text-gray-500">${passengers.map(p => p.name).join(', ')}</p>
        `;
        paymentTotalFare.textContent = `₹ ${totalFare.toFixed(2)}`;
        showPage('page-payment');
    }

    // --- PAYMENT FORM (Now saves to database) ---
    paymentForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        if (!currentUserId) {
            alert("You must be logged in to book a ticket.");
            showLoginModal();
            return;
        }
        
        const pnr = `${Math.floor(100 + Math.random() * 900)}-${Math.floor(1000000 + Math.random() * 9000000)}`;

        const bookingData = {
            pnr: pnr,
            userId: currentUserId,
            bookedAt: Timestamp.now(),
            journeyDate: journeyDateInput.value,
            train: currentBooking.train,
            passengers: currentBooking.passengers,
            isPast: false
        };

        try {
            const bookingsPath = `artifacts/${appId}/users/${currentUserId}/bookings`;
            const docRef = await addDoc(collection(db, bookingsPath), bookingData);
            console.log("Booking saved with ID: ", docRef.id);
            showConfirmation(pnr);
        } catch (error) {
            console.error("Error saving booking: ", error);
            alert(`Booking failed to save: ${error.message}`);
        }
    });

    // --- BOOKING FLOW LOGIC (Part 4 - Confirmation) (Slightly changed) ---
    function showConfirmation(pnr) { 
        const train = currentBooking.train;
        const passengers = currentBooking.passengers;
        
        confirmationPNR.textContent = pnr;
        confirmationJourneySummary.innerHTML = getJourneySummaryHtml(train);
        let passengerHtml = '<p class="text-sm font-medium text-gray-700">Passengers:</p><ul class="list-disc list-inside text-sm text-gray-500">';
        passengers.forEach(p => {
            passengerHtml += `<li>${p.name} (${p.age}, ${p.gender})</li>`;
        });
        passengerHtml += '</ul>';
        confirmationPassengerSummary.innerHTML = passengerHtml;
        
        showPage('page-confirmation');
    }

    confirmationHomeBtn.addEventListener('click', (e) => {
        e.preventDefault();
        showPage('page-search');
    });

} // End of initializeAppLogic()


// ==================================
//  5. APP ENTRY POINT
// ==================================
// This is the code that starts everything.
document.addEventListener('DOMContentLoaded', async () => {
    console.log("DOM content loaded. Setting up Firebase...");
    // First, set up Firebase and authentication
    const firebaseReady = await setupFirebase();

    // After Firebase is ready, initialize all the app's event listeners
    if (firebaseReady && window.firebase) {
        initializeAppLogic();
    } else {
        console.error("Firebase failed to initialize. App logic will not run.");
        // You could show an error on the page to the user here
        document.body.innerHTML = `<div class="p-8 text-center text-red-600">
            <h1 class="text-2xl font-bold">Application Error</h1>
            <p>Could not connect to required services. Please check your connection and refresh the page.</p>
        </div>`;
    }
});