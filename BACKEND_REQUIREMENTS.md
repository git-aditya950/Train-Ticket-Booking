# TrainTrack Backend Requirements - C++ OOP Implementation Guide

## Executive Summary
This document provides a comprehensive analysis of the TrainTrack frontend application and outlines the complete backend requirements for building a C++ REST API server using Object-Oriented Programming principles.

---

## Table of Contents
1. [Frontend Analysis & Current Features](#frontend-analysis--current-features)
2. [Required API Endpoints](#required-api-endpoints)
3. [Data Storage Strategy](#data-storage-strategy)
4. [OOP Class Structure](#oop-class-structure)
5. [Authentication & Authorization](#authentication--authorization)
6. [Data Flow Diagrams](#data-flow-diagrams)
7. [Error Handling](#error-handling)
8. [Additional Recommendations](#additional-recommendations)

---

## Frontend Analysis & Current Features

### 1. **User Authentication System**
The frontend currently uses Firebase for authentication. Users can:
- **Register**: Create new account with name, email, and password
- **Login**: Sign in with email and password
- **Logout**: Sign out and return to anonymous mode
- **Profile Management**: View and update user profile (name, email, phone, password)

**Key Frontend Functions:**
- `handleLogin(e)` - Processes login form submission
- `handleRegister(e)` - Processes registration form submission
- `handleLogout()` - Logs user out
- `handleProfileUpdate(e)` - Updates user profile
- `updateUIForLoggedInUser(user)` - Shows profile menu, hides auth buttons
- `updateUIForLoggedOutUser()` - Shows auth buttons, hides profile menu

---

### 2. **Train Search Functionality**
Users can search for trains between stations with the following features:
- **Search Parameters:**
  - From Station (e.g., "New Delhi (NDLS)")
  - To Station (e.g., "Mumbai (CST)")
  - Journey Date (today to 60 days in advance)

**Current Backend Call:**
```javascript
const CXX_BACKEND_URL = "http://localhost:18080/api/search";
const url = `${CXX_BACKEND_URL}?from=${encodeURIComponent(fromValue)}&to=${encodeURIComponent(toValue)}`;
```

**Expected Response Format:**
```json
{
  "status": "success",
  "trains": [
    {
      "trainNumber": "12001",
      "trainName": "Shatabdi Express",
      "from": "New Delhi (NDLS)",
      "to": "Bhopal (BPL)",
      "departureTime": "06:00",
      "arrivalTime": "14:05",
      "duration": "8h 5m",
      "availability": [
        {
          "class": "CC",
          "status": "AVL 50",
          "price": 1800
        },
        {
          "class": "EC",
          "status": "AVL 10",
          "price": 3200
        }
      ]
    }
  ]
}
```

**Key Frontend Functions:**
- `handleTrainSearch(e)` - Initiates search request
- `displayTrainResults(trains)` - Renders train cards with availability
- `getRandomTrainsFromMock(from, to)` - Fallback for when backend is unavailable

---

### 3. **Booking Flow**
The complete booking process involves multiple steps:

#### **Step 1: Select Train & Class**
- User clicks "Book Now" button for a specific class (e.g., "3A", "SL", "CC")
- Frontend stores: `train`, `selectedClass`, `selectedPrice`

#### **Step 2: Add Passengers**
- User adds passengers with details:
  - Full Name (string)
  - Age (number)
  - Gender (Male/Female/Other)
  - Berth Preference (Lower/Middle/Upper/Side Lower/Side Upper/No Choice)
- Frontend validates: At least 1 passenger required to proceed
- Passengers can be added/removed dynamically

#### **Step 3: Payment (Simulated)**
- Displays booking summary with total fare
- Mock payment form (card number, expiry, CVC)
- Generates PNR (Passenger Name Record) format: `XXX-XXXXXXX` (e.g., `654-3219870`)

#### **Step 4: Confirmation**
- **Seat Assignment**: Frontend assigns seats automatically
  - Format: `CoachType-CoachNumber-SeatNumber` (e.g., "B2-15", "S3-42")
  - Coach Types: SL→S, 3A→B, 2A→A, CC/EC/1A→H
- **Berth Assignment**: Based on seat number (Lower/Middle/Upper/Side Lower/Side Upper)

**Booking Data Structure:**
```javascript
currentBooking = {
  train: {
    trainNumber: "12001",
    trainName: "Shatabdi Express",
    from: "New Delhi (NDLS)",
    to: "Mumbai (CST)",
    departureTime: "06:00",
    arrivalTime: "14:05",
    duration: "8h 5m"
  },
  passengers: [
    {
      name: "John Doe",
      age: 25,
      gender: "Male",
      berth: "Lower",
      assignedSeat: "B2-15",
      assignedBerth: "Lower"
    }
  ],
  selectedClass: {
    class: "3A",
    price: 2500
  },
  journeyDate: "2025-11-20",
  totalFare: 5000,
  pnr: "654-3219870",
  bookingDate: "2025-11-17T10:30:00.000Z",
  status: "Confirmed",
  userId: "user123"
}
```

**Key Frontend Functions:**
- `startBookingProcess(train, selectedClass, price)` - Initiates booking
- `handleAddPassenger(e)` - Adds passenger to list
- `renderPassengerList()` - Updates passenger UI
- `assignSeats(passengers, coachClass)` - Generates seat/berth assignments
- `handlePayment(e)` - Processes payment and saves booking

**Current Firebase Save Operation (To Be Replaced):**
```javascript
// This will be replaced by POST request to C++ backend
// Backend will store booking in memory (std::vector or std::map)
```

---

### 4. **My Bookings Page**
Users can view all their past and upcoming bookings:

**Display Information:**
- Train Name & Number
- Route (From → To)
- PNR Number
- Journey Date
- Class
- Number of Passengers
- Total Fare
- Individual Passenger Details (Name, Age, Seat, Berth)

**Current Firebase Listener (To Be Replaced):**
```javascript
// This will be replaced by GET request to C++ backend
// Backend will return bookings from in-memory storage
```

**Key Frontend Functions:**
- `attachBookingsListener(uid)` - Sets up real-time listener
- `displayUserBookings(bookings)` - Renders booking cards

---

### 5. **Profile Management**
Users can view and update their profile:

**Profile Fields:**
- Full Name (editable)
- Email Address (read-only after registration)
- Phone Number (editable)
- Password (can be changed)

**Current Frontend:**
- Profile data populated from Firebase Auth
- Update operations planned but not fully implemented

---

### 6. **Gemini AI Integration (Optional Feature)**
The frontend includes AI-powered features:
- **Trip Inspiration**: Suggest destinations based on user preferences
- **Trip Planning**: Generate 2-day itineraries for destinations

**Note**: This feature is independent and doesn't require backend support if Gemini API is called directly from frontend.

---

## Required API Endpoints

### Base URL: `http://localhost:18080/api`

---

### **1. Authentication Endpoints**

#### **POST /auth/register**
Create a new user account.

**Request Body:**
```json
{
  "name": "John Doe",
  "email": "john@example.com",
  "password": "SecurePassword123",
  "phone": "+91 98765 43210"
}
```

**Response (Success - 201 Created):**
```json
{
  "status": "success",
  "message": "User registered successfully",
  "data": {
    "userId": "user_abc123",
    "name": "John Doe",
    "email": "john@example.com",
    "phone": "+91 98765 43210",
    "createdAt": "2025-11-17T10:30:00.000Z"
  },
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
}
```

**Response (Error - 400 Bad Request):**
```json
{
  "status": "error",
  "message": "Email already exists"
}
```

**Validation Rules:**
- Email must be valid and unique
- Password minimum 8 characters
- Name required
- Phone number optional but should match pattern if provided

---

#### **POST /auth/login**
Authenticate user and return token.

**Request Body:**
```json
{
  "email": "john@example.com",
  "password": "SecurePassword123"
}
```

**Response (Success - 200 OK):**
```json
{
  "status": "success",
  "message": "Login successful",
  "data": {
    "userId": "user_abc123",
    "name": "John Doe",
    "email": "john@example.com",
    "phone": "+91 98765 43210"
  },
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
}
```

**Response (Error - 401 Unauthorized):**
```json
{
  "status": "error",
  "message": "Invalid email or password"
}
```

---

#### **POST /auth/logout**
Invalidate user token (optional if using JWT - can be handled client-side).

**Request Headers:**
```
Authorization: Bearer <token>
```

**Response (Success - 200 OK):**
```json
{
  "status": "success",
  "message": "Logged out successfully"
}
```

---

#### **GET /auth/profile**
Get current user's profile.

**Request Headers:**
```
Authorization: Bearer <token>
```

**Response (Success - 200 OK):**
```json
{
  "status": "success",
  "data": {
    "userId": "user_abc123",
    "name": "John Doe",
    "email": "john@example.com",
    "phone": "+91 98765 43210",
    "createdAt": "2025-11-17T10:30:00.000Z"
  }
}
```

---

#### **PUT /auth/profile**
Update user profile.

**Request Headers:**
```
Authorization: Bearer <token>
```

**Request Body:**
```json
{
  "name": "John Smith",
  "phone": "+91 99999 99999",
  "currentPassword": "OldPassword123",
  "newPassword": "NewPassword456"
}
```

**Response (Success - 200 OK):**
```json
{
  "status": "success",
  "message": "Profile updated successfully",
  "data": {
    "userId": "user_abc123",
    "name": "John Smith",
    "email": "john@example.com",
    "phone": "+91 99999 99999"
  }
}
```

**Notes:**
- `currentPassword` required only if changing password
- Email cannot be changed
- Phone can be null/empty

---

### **2. Train Search Endpoint**

#### **GET /search**
Search for trains between two stations.

**Query Parameters:**
- `from` (required): Source station (e.g., "New Delhi (NDLS)")
- `to` (required): Destination station (e.g., "Mumbai (CST)")
- `date` (optional): Journey date (YYYY-MM-DD format, defaults to today)

**Example Request:**
```
GET /api/search?from=New%20Delhi%20(NDLS)&to=Mumbai%20(CST)&date=2025-11-20
```

**Response (Success - 200 OK):**
```json
{
  "status": "success",
  "count": 5,
  "trains": [
    {
      "trainNumber": "12951",
      "trainName": "Mumbai Rajdhani",
      "from": "New Delhi (NDLS)",
      "to": "Mumbai (CST)",
      "departureTime": "17:00",
      "arrivalTime": "08:35",
      "duration": "15h 35m",
      "availability": [
        {
          "class": "SL",
          "status": "AVL 150",
          "price": 1200,
          "availableSeats": 150
        },
        {
          "class": "3A",
          "status": "AVL 22",
          "price": 2500,
          "availableSeats": 22
        },
        {
          "class": "2A",
          "status": "AVL 8",
          "price": 3500,
          "availableSeats": 8
        },
        {
          "class": "1A",
          "status": "WL 2",
          "price": 4800,
          "availableSeats": 0,
          "waitingList": 2
        }
      ]
    }
  ]
}
```

**Response (No Trains Found - 200 OK):**
```json
{
  "status": "success",
  "count": 0,
  "trains": [],
  "message": "No trains available for this route"
}
```

**Response (Error - 400 Bad Request):**
```json
{
  "status": "error",
  "message": "Missing required parameter: from"
}
```

**Class Codes:**
- `2S`: Second Sitting
- `SL`: Sleeper Class
- `CC`: Chair Car
- `3A`: Third AC
- `2A`: Second AC
- `1A`: First AC
- `EC`: Executive Chair Car

**Status Format:**
- `AVL X`: X seats available
- `RAC X`: X RAC (Reservation Against Cancellation)
- `WL X`: X in waiting list

---

### **3. Booking Endpoints**

#### **POST /bookings**
Create a new booking.

**Request Headers:**
```
Authorization: Bearer <token>
Content-Type: application/json
```

**Request Body:**
```json
{
  "train": {
    "trainNumber": "12951",
    "trainName": "Mumbai Rajdhani",
    "from": "New Delhi (NDLS)",
    "to": "Mumbai (CST)",
    "departureTime": "17:00",
    "arrivalTime": "08:35",
    "duration": "15h 35m"
  },
  "selectedClass": {
    "class": "3A",
    "price": 2500
  },
  "journeyDate": "2025-11-20",
  "passengers": [
    {
      "name": "John Doe",
      "age": 25,
      "gender": "Male",
      "berth": "Lower"
    },
    {
      "name": "Jane Doe",
      "age": 23,
      "gender": "Female",
      "berth": "Upper"
    }
  ]
}
```

**Response (Success - 201 Created):**
```json
{
  "status": "success",
  "message": "Booking confirmed successfully",
  "data": {
    "bookingId": "booking_xyz789",
    "pnr": "654-3219870",
    "userId": "user_abc123",
    "train": {
      "trainNumber": "12951",
      "trainName": "Mumbai Rajdhani",
      "from": "New Delhi (NDLS)",
      "to": "Mumbai (CST)",
      "departureTime": "17:00",
      "arrivalTime": "08:35",
      "duration": "15h 35m"
    },
    "selectedClass": {
      "class": "3A",
      "price": 2500
    },
    "journeyDate": "2025-11-20",
    "passengers": [
      {
        "name": "John Doe",
        "age": 25,
        "gender": "Male",
        "berth": "Lower",
        "assignedSeat": "B3-15",
        "assignedBerth": "Lower"
      },
      {
        "name": "Jane Doe",
        "age": 23,
        "gender": "Female",
        "berth": "Upper",
        "assignedSeat": "B3-16",
        "assignedBerth": "Upper"
      }
    ],
    "totalFare": 5000,
    "bookingDate": "2025-11-17T10:30:00.000Z",
    "status": "Confirmed"
  }
}
```

**Response (Error - 400 Bad Request):**
```json
{
  "status": "error",
  "message": "Insufficient seats available. Only 1 seat remaining in 3A class."
}
```

**Validation Rules:**
- User must be authenticated
- At least 1 passenger required
- Maximum 6 passengers per booking
- Selected class must have available seats
- Journey date must be today or future (up to 60 days)
- Passenger age must be between 1 and 120

---

#### **GET /bookings**
Get all bookings for the authenticated user.

**Request Headers:**
```
Authorization: Bearer <token>
```

**Query Parameters (Optional):**
- `status`: Filter by status (Confirmed/Cancelled/Completed)
- `limit`: Number of results (default: 50)
- `offset`: Pagination offset (default: 0)

**Example Request:**
```
GET /api/bookings?status=Confirmed&limit=10
```

**Response (Success - 200 OK):**
```json
{
  "status": "success",
  "count": 3,
  "data": [
    {
      "bookingId": "booking_xyz789",
      "pnr": "654-3219870",
      "train": {
        "trainNumber": "12951",
        "trainName": "Mumbai Rajdhani",
        "from": "New Delhi (NDLS)",
        "to": "Mumbai (CST)",
        "departureTime": "17:00",
        "arrivalTime": "08:35",
        "duration": "15h 35m"
      },
      "selectedClass": {
        "class": "3A",
        "price": 2500
      },
      "journeyDate": "2025-11-20",
      "passengers": [
        {
          "name": "John Doe",
          "age": 25,
          "gender": "Male",
          "berth": "Lower",
          "assignedSeat": "B3-15",
          "assignedBerth": "Lower"
        }
      ],
      "totalFare": 2500,
      "bookingDate": "2025-11-17T10:30:00.000Z",
      "status": "Confirmed"
    }
  ]
}
```

---

#### **GET /bookings/:bookingId**
Get details of a specific booking.

**Request Headers:**
```
Authorization: Bearer <token>
```

**Example Request:**
```
GET /api/bookings/booking_xyz789
```

**Response (Success - 200 OK):**
```json
{
  "status": "success",
  "data": {
    "bookingId": "booking_xyz789",
    "pnr": "654-3219870",
    "userId": "user_abc123",
    "train": { /* ... */ },
    "passengers": [ /* ... */ ],
    "totalFare": 2500,
    "bookingDate": "2025-11-17T10:30:00.000Z",
    "status": "Confirmed"
  }
}
```

**Response (Error - 404 Not Found):**
```json
{
  "status": "error",
  "message": "Booking not found"
}
```

---

#### **DELETE /bookings/:bookingId**
Cancel a booking.

**Request Headers:**
```
Authorization: Bearer <token>
```

**Response (Success - 200 OK):**
```json
{
  "status": "success",
  "message": "Booking cancelled successfully",
  "data": {
    "bookingId": "booking_xyz789",
    "pnr": "654-3219870",
    "status": "Cancelled",
    "refundAmount": 2000
  }
}
```

**Business Rules:**
- Can only cancel bookings at least 24 hours before journey
- Refund: 80% of fare if cancelled >24h before journey
- User can only cancel their own bookings

---

### **4. Station Endpoints (Optional)**

#### **GET /stations**
Get list of all stations (for autocomplete).

**Response (Success - 200 OK):**
```json
{
  "status": "success",
  "count": 68,
  "data": [
    {
      "stationCode": "NDLS",
      "stationName": "New Delhi",
      "displayName": "New Delhi (NDLS)"
    },
    {
      "stationCode": "CST",
      "stationName": "Mumbai",
      "displayName": "Mumbai (CST)"
    }
  ]
}
```

---

## Data Storage Strategy

### **In-Memory Storage Approach**

All data will be stored in memory using C++ STL containers. No database is required.

---

### **Storage Containers**

#### **1. Users Storage**
```cpp
// Store users in a map with email as key for fast lookup
std::unordered_map<std::string, User> usersByEmail;

// Store users by userId for token validation
std::unordered_map<std::string, User> usersById;
```

---

#### **2. Trains Storage**
```cpp
// Store trains in a map with train number as key
std::unordered_map<std::string, Train> trains;

// Store trains by route for fast search
// Key format: "from_station|to_station"
std::unordered_map<std::string, std::vector<std::string>> trainsByRoute;
```

---

#### **3. Bookings Storage**
```cpp
// Store bookings by bookingId
std::unordered_map<std::string, Booking> bookingsById;

// Store bookings by userId for fast user lookup
std::unordered_map<std::string, std::vector<std::string>> bookingsByUser;

// Store bookings by PNR
std::unordered_map<std::string, std::string> pnrToBookingId;
```

---

#### **4. Sessions Storage (For JWT)**
```cpp
// Store active sessions/tokens (optional - JWT can be stateless)
std::unordered_map<std::string, std::string> activeSessions; // token -> userId
```

---

#### **5. Stations Storage**
```cpp
// Store station list for autocomplete
std::vector<Station> stations;
```

---

### **Data Initialization**

On server startup, initialize with sample data:

```cpp
void initializeData() {
    // Load sample trains
    loadSampleTrains();
    
    // Load station list
    loadStations();
    
    // Can optionally load from JSON files for persistence between restarts
}
```

---

### **Data Persistence (Optional)**

If you want data to survive server restarts:

```cpp
// Save data to JSON files periodically
void saveDataToFile() {
    saveUsersToJson("data/users.json");
    saveBookingsToJson("data/bookings.json");
    saveTrainsToJson("data/trains.json");
}

// Load data from JSON files on startup
void loadDataFromFile() {
    loadUsersFromJson("data/users.json");
    loadBookingsFromJson("data/bookings.json");
    loadTrainsFromJson("data/trains.json");
}
```

---

## OOP Class Structure

### **Architecture: MVC Pattern**
- **Models**: Data entities and in-memory storage operations
- **Views**: JSON response formatting
- **Controllers**: Business logic and request handling
- **Services**: Reusable business logic
- **Middleware**: Authentication, validation, error handling

---

### **Class Diagram**

```
┌──────────────────┐
│   HTTPServer     │ (Main server class)
├──────────────────┤
│ - port           │
│ - router         │
├──────────────────┤
│ + start()        │
│ + handleRequest()│
└──────────────────┘
         │
         │ uses
         ▼
┌──────────────────┐
│     Router       │ (Routes requests to controllers)
├──────────────────┤
│ - routes         │
├──────────────────┤
│ + addRoute()     │
│ + route()        │
└──────────────────┘
         │
         │ routes to
         ▼
┌────────────────────────────────────────────┐
│          Controllers                       │
├────────────────────────────────────────────┤
│ - AuthController                           │
│ - TrainController                          │
│ - BookingController                        │
├────────────────────────────────────────────┤
│ + register(), login(), logout()            │
│ + searchTrains()                           │
│ + createBooking(), getBookings()           │
└────────────────────────────────────────────┘
         │
         │ uses
         ▼
┌────────────────────────────────────────────┐
│           Services                         │
├────────────────────────────────────────────┤
│ - AuthService                              │
│ - BookingService                           │
│ - SeatAllocationService                    │
├────────────────────────────────────────────┤
│ + authenticateUser()                       │
│ + validateBooking()                        │
│ + assignSeats()                            │
└────────────────────────────────────────────┘
         │
         │ uses
         ▼
┌────────────────────────────────────────────┐
│            Models                          │
├────────────────────────────────────────────┤
│ - User                                     │
│ - Train                                    │
│ - Booking                                  │
│ - Passenger                                │
├────────────────────────────────────────────┤
│ + save(), find(), update(), delete()       │
└────────────────────────────────────────────┘
         │
         │ stores in
         ▼
┌────────────────────────────────────────────┐
│       DataStore (In-Memory)                │
├────────────────────────────────────────────┤
│ - std::unordered_map for storage           │
│ - Thread-safe operations                   │
├────────────────────────────────────────────┤
│ + store(), retrieve(), update(), remove()  │
└────────────────────────────────────────────┘
```

---

### **Detailed Class Definitions**

#### **1. User Class (Model)**

```cpp
class User {
private:
    std::string userId;
    std::string name;
    std::string email;
    std::string passwordHash;
    std::string phone;
    std::string createdAt;
    std::string updatedAt;

public:
    // Constructors
    User();
    User(const std::string& name, const std::string& email, 
         const std::string& password, const std::string& phone);
    
    // Getters
    std::string getUserId() const;
    std::string getName() const;
    std::string getEmail() const;
    std::string getPhone() const;
    
    // Setters
    void setName(const std::string& name);
    void setPhone(const std::string& phone);
    void setPassword(const std::string& password);
    
    // Storage Operations (In-Memory)
    bool save();                        // Store in memory
    static User* findByEmail(const std::string& email);
    static User* findById(const std::string& userId);
    bool update();                      // Update in memory
    bool deleteUser();
    
    // Authentication
    bool verifyPassword(const std::string& password) const;
    std::string generateToken() const;
    
    // Validation
    bool isValid() const;
    std::string getValidationErrors() const;
    
    // Serialization
    nlohmann::json toJson() const;
    static User fromJson(const nlohmann::json& json);
};
```

---

#### **2. Train Class (Model)**

```cpp
class TrainAvailability {
public:
    std::string classCode;
    int totalSeats;
    int availableSeats;
    double price;
    
    std::string getStatus() const;
    nlohmann::json toJson() const;
};

class Train {
private:
    std::string trainNumber;
    std::string trainName;
    std::string fromStation;
    std::string toStation;
    std::string departureTime;
    std::string arrivalTime;
    std::string duration;
    std::vector<TrainAvailability> availability;

public:
    // Constructors
    Train();
    Train(const std::string& number, const std::string& name);
    
    // Getters
    std::string getTrainNumber() const;
    std::string getTrainName() const;
    std::string getFromStation() const;
    std::string getToStation() const;
    std::vector<TrainAvailability> getAvailability() const;
    
    // Storage Operations (In-Memory)
    bool save();
    static std::vector<Train> searchTrains(
        const std::string& from, 
        const std::string& to,
        const std::string& date
    );
    static Train* findByNumber(const std::string& trainNumber);
    bool update();
    
    // Availability Management
    bool updateAvailability(const std::string& classCode, int seatsToBook);
    int getAvailableSeats(const std::string& classCode) const;
    double getPrice(const std::string& classCode) const;
    
    // Serialization
    nlohmann::json toJson() const;
    static Train fromJson(const nlohmann::json& json);
};
```

---

#### **3. Passenger Class (Model)**

```cpp
class Passenger {
private:
    int passengerId;
    std::string bookingId;
    std::string name;
    int age;
    std::string gender;
    std::string berthPreference;
    std::string assignedSeat;
    std::string assignedBerth;

public:
    // Constructors
    Passenger();
    Passenger(const std::string& name, int age, 
              const std::string& gender, const std::string& berth);
    
    // Getters
    std::string getName() const;
    int getAge() const;
    std::string getGender() const;
    std::string getAssignedSeat() const;
    std::string getAssignedBerth() const;
    
    // Setters
    void setBookingId(const std::string& id);
    void setAssignedSeat(const std::string& seat);
    void setAssignedBerth(const std::string& berth);
    
    // Storage Operations (In-Memory)
    bool save();
    static std::vector<Passenger> findByBookingId(const std::string& bookingId);
    
    // Validation
    bool isValid() const;
    
    // Serialization
    nlohmann::json toJson() const;
    static Passenger fromJson(const nlohmann::json& json);
};
```

---

#### **4. Booking Class (Model)**

```cpp
class Booking {
private:
    std::string bookingId;
    std::string pnr;
    std::string userId;
    Train train;
    std::string classCode;
    double pricePerPassenger;
    double totalFare;
    std::string journeyDate;
    std::string bookingDate;
    std::string status;
    std::vector<Passenger> passengers;

public:
    // Constructors
    Booking();
    Booking(const std::string& userId, const Train& train, 
            const std::string& classCode);
    
    // Getters
    std::string getBookingId() const;
    std::string getPnr() const;
    std::string getUserId() const;
    std::string getStatus() const;
    std::vector<Passenger> getPassengers() const;
    double getTotalFare() const;
    
    // Setters
    void addPassenger(const Passenger& passenger);
    void setJourneyDate(const std::string& date);
    void setStatus(const std::string& status);
    
    // Business Logic
    void calculateTotalFare();
    std::string generatePnr();
    bool assignSeats();
    
    // Storage Operations (In-Memory)
    bool save();
    static std::vector<Booking> findByUserId(const std::string& userId);
    static Booking* findById(const std::string& bookingId);
    static Booking* findByPnr(const std::string& pnr);
    bool update();
    bool cancel();
    
    // Validation
    bool isValid() const;
    std::string getValidationErrors() const;
    
    // Serialization
    nlohmann::json toJson(bool includePassengers = true) const;
    static Booking fromJson(const nlohmann::json& json);
};
```

---

#### **5. AuthController Class (Controller)**

```cpp
class AuthController {
private:
    AuthService* authService;

public:
    AuthController(AuthService* service);
    
    // Route Handlers
    Response handleRegister(const Request& request);
    Response handleLogin(const Request& request);
    Response handleLogout(const Request& request);
    Response handleGetProfile(const Request& request);
    Response handleUpdateProfile(const Request& request);
    
private:
    // Helper Methods
    bool validateRegisterInput(const nlohmann::json& data, std::string& error);
    bool validateLoginInput(const nlohmann::json& data, std::string& error);
    User* getUserFromToken(const std::string& token);
};
```

---

#### **6. TrainController Class (Controller)**

```cpp
class TrainController {
public:
    TrainController();
    
    // Route Handlers
    Response handleSearch(const Request& request);
    Response handleGetStations(const Request& request);
    
private:
    // Helper Methods
    bool validateSearchParams(const Request& request, 
                             std::string& from, 
                             std::string& to,
                             std::string& date,
                             std::string& error);
};
```

---

#### **7. BookingController Class (Controller)**

```cpp
class BookingController {
private:
    BookingService* bookingService;
    SeatAllocationService* seatService;

public:
    BookingController(BookingService* bookingService, 
                     SeatAllocationService* seatService);
    
    // Route Handlers
    Response handleCreateBooking(const Request& request);
    Response handleGetBookings(const Request& request);
    Response handleGetBookingById(const Request& request);
    Response handleCancelBooking(const Request& request);
    
private:
    // Helper Methods
    bool validateBookingInput(const nlohmann::json& data, std::string& error);
    User* getUserFromToken(const Request& request);
    bool checkSeatAvailability(const std::string& trainNumber, 
                              const std::string& classCode, 
                              int numPassengers);
};
```

---

#### **8. BookingService Class (Service)**

```cpp
class BookingService {
public:
    BookingService();
    
    // Business Logic
    Booking* createBooking(User* user, 
                          const Train& train,
                          const std::string& classCode,
                          const std::string& journeyDate,
                          const std::vector<Passenger>& passengers);
    
    std::vector<Booking> getUserBookings(const std::string& userId,
                                        const std::string& status = "");
    
    bool cancelBooking(const std::string& bookingId, 
                      const std::string& userId);
    
    double calculateRefund(const Booking& booking);
    
private:
    bool validateBookingRules(const Booking& booking, std::string& error);
    bool updateTrainAvailability(const std::string& trainNumber,
                                const std::string& classCode,
                                int seatsToDeduct);
};
```

---

#### **9. SeatAllocationService Class (Service)**

```cpp
class SeatAllocationService {
public:
    SeatAllocationService();
    
    // Seat Assignment Logic
    bool assignSeats(std::vector<Passenger>& passengers, 
                    const std::string& classCode,
                    const std::string& trainNumber);
    
private:
    std::string generateCoachPrefix(const std::string& classCode);
    int getRandomCoachNumber();
    std::string getBerthType(int seatNumber, const std::string& classCode);
    int getNextAvailableSeat(const std::string& trainNumber, 
                            const std::string& coach);
};
```

---

#### **10. AuthService Class (Service)**

```cpp
class AuthService {
public:
    AuthService();
    
    // Authentication Logic
    User* registerUser(const std::string& name, 
                      const std::string& email,
                      const std::string& password, 
                      const std::string& phone);
    
    std::pair<User*, std::string> loginUser(const std::string& email, 
                                           const std::string& password);
    
    bool logoutUser(const std::string& token);
    
    User* validateToken(const std::string& token);
    
    bool updateProfile(User* user, 
                      const nlohmann::json& updates);
    
private:
    // Security
    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, 
                       const std::string& hash);
    std::string generateToken(const User& user);
    std::string generateUserId();
};
```

---

#### **11. DataStore Class (In-Memory Storage)**

```cpp
class DataStore {
private:
    // User storage
    std::unordered_map<std::string, User> usersByEmail;
    std::unordered_map<std::string, User> usersById;
    
    // Train storage
    std::unordered_map<std::string, Train> trains;
    std::unordered_map<std::string, std::vector<std::string>> trainsByRoute;
    
    // Booking storage
    std::unordered_map<std::string, Booking> bookingsById;
    std::unordered_map<std::string, std::vector<std::string>> bookingsByUser;
    std::unordered_map<std::string, std::string> pnrToBookingId;
    
    // Session storage
    std::unordered_map<std::string, std::string> activeSessions;
    
    // Thread safety
    std::mutex userMutex;
    std::mutex trainMutex;
    std::mutex bookingMutex;
    std::mutex sessionMutex;
    
    // Singleton instance
    static DataStore* instance;
    DataStore();

public:
    static DataStore* getInstance();
    
    // User Operations
    bool addUser(const User& user);
    User* findUserByEmail(const std::string& email);
    User* findUserById(const std::string& userId);
    bool updateUser(const User& user);
    bool deleteUser(const std::string& userId);
    
    // Train Operations
    bool addTrain(const Train& train);
    Train* findTrainByNumber(const std::string& trainNumber);
    std::vector<Train> searchTrains(const std::string& from, const std::string& to);
    bool updateTrain(const Train& train);
    
    // Booking Operations
    bool addBooking(const Booking& booking);
    Booking* findBookingById(const std::string& bookingId);
    Booking* findBookingByPnr(const std::string& pnr);
    std::vector<Booking> findBookingsByUser(const std::string& userId);
    bool updateBooking(const Booking& booking);
    bool deleteBooking(const std::string& bookingId);
    
    // Session Operations
    void addSession(const std::string& token, const std::string& userId);
    std::string getUserIdFromToken(const std::string& token);
    void removeSession(const std::string& token);
    
    // Data Persistence (Optional)
    bool saveToFile(const std::string& directory);
    bool loadFromFile(const std::string& directory);
    
    // Initialize with sample data
    void initializeSampleData();
};
```

---

#### **12. HTTPServer Class (Server)**

```cpp
class HTTPServer {
private:
    int port;
    Router* router;
    bool running;

public:
    HTTPServer(int port);
    ~HTTPServer();
    
    // Server Control
    void start();
    void stop();
    bool isRunning() const;
    
    // Configuration
    void setRouter(Router* router);
    void enableCORS();
    
private:
    void handleRequest(const Request& request, Response& response);
    void sendResponse(int clientSocket, const Response& response);
};
```

---

#### **13. Router Class (Routing)**

```cpp
class Router {
private:
    std::map<std::string, Controller*> routes;

public:
    Router();
    
    // Route Registration
    void addRoute(const std::string& method, 
                 const std::string& path, 
                 Controller* controller,
                 Response (Controller::*handler)(const Request&));
    
    // Request Handling
    Response route(const Request& request);
    
    // Middleware
    void addMiddleware(Middleware* middleware);
    
private:
    bool matchRoute(const std::string& pattern, 
                   const std::string& path,
                   std::map<std::string, std::string>& params);
};
```

---

### **Additional Helper Classes**

#### **Request & Response Classes**

```cpp
class Request {
public:
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queryParams;
    std::map<std::string, std::string> pathParams;
    nlohmann::json body;
    
    std::string getHeader(const std::string& key) const;
    std::string getQueryParam(const std::string& key) const;
    std::string getPathParam(const std::string& key) const;
};

class Response {
public:
    int statusCode;
    std::map<std::string, std::string> headers;
    nlohmann::json body;
    
    Response(int code = 200);
    
    void setJson(const nlohmann::json& json);
    void setError(const std::string& message, int code = 400);
    void setSuccess(const nlohmann::json& data, 
                   const std::string& message = "");
    std::string toString() const;
};
```

---

## Authentication & Authorization

### **JWT (JSON Web Token) Implementation**

#### **Token Structure**
```
Header.Payload.Signature
```

**Payload Example:**
```json
{
  "userId": "user_abc123",
  "email": "john@example.com",
  "iat": 1700000000,
  "exp": 1700086400
}
```

#### **Token Generation (Pseudo-code)**
```cpp
std::string AuthService::generateToken(const User& user) {
    nlohmann::json payload = {
        {"userId", user.getUserId()},
        {"email", user.getEmail()},
        {"iat", std::time(nullptr)},
        {"exp", std::time(nullptr) + 86400} // 24 hours
    };
    
    std::string secret = "YOUR_SECRET_KEY";
    return jwt::encode(payload, secret);
}
```

#### **Token Validation**
```cpp
User* AuthService::validateToken(const std::string& token) {
    try {
        auto decoded = jwt::decode(token);
        std::string userId = decoded.get("userId");
        return User::findById(userId);
    } catch (const std::exception& e) {
        return nullptr; // Invalid token
    }
}
```

---

### **Middleware for Protected Routes**

```cpp
class AuthMiddleware {
public:
    static bool authenticate(Request& request, Response& response) {
        std::string token = request.getHeader("Authorization");
        
        if (token.empty() || token.substr(0, 7) != "Bearer ") {
            response.setError("Missing or invalid authorization header", 401);
            return false;
        }
        
        token = token.substr(7); // Remove "Bearer "
        
        AuthService authService;
        User* user = authService.validateToken(token);
        
        if (!user) {
            response.setError("Invalid or expired token", 401);
            return false;
        }
        
        // Attach user to request for use in controllers
        request.user = user;
        return true;
    }
};
```

---

## Data Flow Diagrams

### **1. User Registration Flow**

```
Frontend                Backend                 Database
   |                       |                        |
   |--POST /auth/register->|                        |
   |   (name, email, pwd)  |                        |
   |                       |--Validate Input------->|
   |                       |                        |
   |                       |--Check Email Exists--->|
   |                       |<-Email Not Found-------|
   |                       |                        |
   |                       |--Hash Password-------->|
   |                       |                        |
   |                       |--INSERT INTO users---->|
   |                       |<-user_id---------------|
   |                       |                        |
   |                       |--Generate JWT Token--->|
   |                       |                        |
   |<-201 Created----------|                        |
   |  {user, token}        |                        |
   |                       |                        |
   |--Store Token--------->|                        |
   |  in localStorage      |                        |
```

---

### **2. Train Search Flow**

```
Frontend                Backend                 Database
   |                       |                        |
   |--GET /api/search----->|                        |
   |   ?from=X&to=Y        |                        |
   |                       |--Validate Params------>|
   |                       |                        |
   |                       |--SELECT trains-------->|
   |                       |   WHERE from=X         |
   |                       |   AND to=Y             |
   |                       |<-train results---------|
   |                       |                        |
   |                       |--Get Availability----->|
   |                       |   JOIN train_avail     |
   |                       |<-availability data-----|
   |                       |                        |
   |                       |--Format Response------>|
   |                       |                        |
   |<-200 OK---------------|                        |
   |  {trains: [...]}      |                        |
   |                       |                        |
   |--Display Results----->|                        |
```

---

### **3. Booking Creation Flow**

```
Frontend                Backend                 Database
   |                       |                        |
   |--POST /bookings------>|                        |
   |   + Auth Token        |                        |
   |   {train, passengers} |                        |
   |                       |--Validate Token------->|
   |                       |<-user authenticated----|
   |                       |                        |
   |                       |--Check Availability--->|
   |                       |<-seats available-------|
   |                       |                        |
   |                       |--BEGIN TRANSACTION---->|
   |                       |                        |
   |                       |--Assign Seats--------->|
   |                       |                        |
   |                       |--Generate PNR--------->|
   |                       |                        |
   |                       |--INSERT booking------->|
   |                       |<-booking_id------------|
   |                       |                        |
   |                       |--INSERT passengers---->|
   |                       |<-success---------------|
   |                       |                        |
   |                       |--UPDATE availability-->|
   |                       |   (decrease seats)     |
   |                       |<-updated---------------|
   |                       |                        |
   |                       |--COMMIT TRANSACTION--->|
   |                       |                        |
   |<-201 Created----------|                        |
   |  {booking details}    |                        |
   |                       |                        |
   |--Show Confirmation--->|                        |
```

---

### **4. Get Bookings Flow**

```
Frontend                Backend                 Database
   |                       |                        |
   |--GET /bookings------->|                        |
   |   + Auth Token        |                        |
   |                       |--Validate Token------->|
   |                       |<-user_id---------------|
   |                       |                        |
   |                       |--SELECT bookings------>|
   |                       |   WHERE user_id=X      |
   |                       |<-booking records-------|
   |                       |                        |
   |                       |--For each booking----->|
   |                       |  SELECT passengers---->|
   |                       |<-passenger data--------|
   |                       |                        |
   |                       |--Format Response------>|
   |                       |                        |
   |<-200 OK---------------|                        |
   |  {bookings: [...]}    |                        |
   |                       |                        |
   |--Display Bookings---->|                        |
```

---

## Error Handling

### **Standard Error Response Format**

```json
{
  "status": "error",
  "message": "Human-readable error message",
  "code": "ERROR_CODE",
  "details": {
    "field": "Specific error details (optional)"
  }
}
```

---

### **HTTP Status Codes**

| Code | Meaning | Usage |
|------|---------|-------|
| 200 | OK | Successful GET/PUT/DELETE |
| 201 | Created | Successful POST (resource created) |
| 400 | Bad Request | Invalid input/validation error |
| 401 | Unauthorized | Missing/invalid token |
| 403 | Forbidden | User not allowed to access resource |
| 404 | Not Found | Resource doesn't exist |
| 409 | Conflict | Duplicate resource (e.g., email exists) |
| 500 | Internal Server Error | Server-side error |

---

### **Error Codes**

```cpp
enum ErrorCode {
    VALIDATION_ERROR = 1001,
    EMAIL_EXISTS = 1002,
    INVALID_CREDENTIALS = 1003,
    TOKEN_EXPIRED = 1004,
    TOKEN_INVALID = 1005,
    BOOKING_NOT_FOUND = 2001,
    INSUFFICIENT_SEATS = 2002,
    INVALID_DATE = 2003,
    BOOKING_CANNOT_CANCEL = 2004,
    STORAGE_ERROR = 5001,
    UNKNOWN_ERROR = 5999
};
```

---

### **Error Handler Class**

```cpp
class ErrorHandler {
public:
    static Response handleError(const std::exception& e, int defaultCode = 500) {
        Response response(defaultCode);
        
        if (auto* validationErr = dynamic_cast<const ValidationException*>(&e)) {
            response.setError(validationErr->what(), 400);
        } 
        else if (auto* authErr = dynamic_cast<const AuthException*>(&e)) {
            response.setError(authErr->what(), 401);
        }
        else if (auto* storageErr = dynamic_cast<const StorageException*>(&e)) {
            response.setError("Storage error", 500);
            // Log detailed error internally
        }
        else {
            response.setError("Internal server error", 500);
        }
        
        return response;
    }
};
```

---

## Additional Recommendations

### **1. Libraries to Use**

#### **HTTP Server**
- **cpp-httplib** (https://github.com/yhirose/cpp-httplib)
  - Simple header-only HTTP/HTTPS library
  - Easy to integrate

#### **JSON Parsing**
- **nlohmann/json** (https://github.com/nlohmann/json)
  - Modern C++ JSON library
  - Intuitive API

#### **JWT Authentication**
- **jwt-cpp** (https://github.com/Thalhammer/jwt-cpp)
  - JWT creation and validation

#### **Password Hashing**
- **bcrypt** (https://github.com/hilch/Bcrypt.cpp)
  - Secure password hashing

#### **Threading (Built-in C++)**
- **std::mutex** and **std::lock_guard**
  - Thread-safe operations for in-memory storage
  - Prevent race conditions

---

### **2. Testing Strategy**

#### **Unit Tests**
- Test individual classes (User, Train, Booking)
- Test services (AuthService, BookingService)
- Use **Google Test** framework

#### **Integration Tests**
- Test API endpoints
- Test in-memory storage operations
- Test authentication flow
- Test thread safety

#### **Test Data**
- Initialize in-memory storage with sample trains
- Create test users
- Mock payment processing

---

### **3. Configuration Management**

Create a `config.json` file:

```json
{
  "server": {
    "host": "localhost",
    "port": 18080
  },
  "storage": {
    "persistToFile": false,
    "dataDirectory": "./data"
  },
  "security": {
    "jwtSecret": "your-secret-key-here",
    "tokenExpiry": 86400,
    "bcryptRounds": 10
  },
  "cors": {
    "enabled": true,
    "allowOrigin": "*"
  }
}
```

---

### **4. CORS Configuration**

For frontend to communicate with backend:

```cpp
void HTTPServer::handleRequest(const Request& request, Response& response) {
    // Add CORS headers
    response.headers["Access-Control-Allow-Origin"] = "*";
    response.headers["Access-Control-Allow-Methods"] = "GET, POST, PUT, DELETE, OPTIONS";
    response.headers["Access-Control-Allow-Headers"] = "Content-Type, Authorization";
    
    // Handle preflight
    if (request.method == "OPTIONS") {
        response.statusCode = 204;
        return;
    }
    
    // Route request
    // ...
}
```

---

### **5. Logging**

Implement logging for debugging and monitoring:

```cpp
class Logger {
public:
    enum Level { DEBUG, INFO, WARNING, ERROR };
    
    static void log(Level level, const std::string& message) {
        std::string timestamp = getCurrentTimestamp();
        std::string levelStr = getLevelString(level);
        
        std::cout << "[" << timestamp << "] " 
                  << "[" << levelStr << "] " 
                  << message << std::endl;
        
        // Also write to file
        writeToFile(timestamp, levelStr, message);
    }
    
private:
    static std::string getCurrentTimestamp();
    static std::string getLevelString(Level level);
    static void writeToFile(const std::string& timestamp,
                           const std::string& level,
                           const std::string& message);
};
```

---

### **6. Performance Considerations**

- **Thread Safety**: Use mutexes for concurrent access to shared maps
- **Fast Lookups**: Use std::unordered_map for O(1) average lookup time
- **Memory Management**: Consider implementing object pooling for frequently created objects
- **Pagination**: Limit number of results returned
- **Read-Write Locks**: Use std::shared_mutex for better concurrent read performance
- **Data Structure Choice**: Use appropriate STL containers (unordered_map vs map vs vector)

---

### **7. Security Best Practices**

1. **Password Security**
   - Use bcrypt with salt
   - Minimum 8 characters
   - Enforce password strength

2. **Input Injection Prevention**
   - Use parameterized operations
   - Validate and sanitize all inputs
   - Escape special characters when necessary

3. **Token Security**
   - Use HTTPS in production
   - Set token expiration
   - Implement token refresh mechanism

4. **Rate Limiting**
   - Limit requests per IP
   - Prevent brute force attacks

5. **Input Validation**
   - Validate all user inputs
   - Sanitize special characters
   - Check data types and ranges
   - Prevent code injection

---

### **8. Deployment Considerations**

- **Environment Variables**: Store secrets (JWT secret)
- **Docker**: Containerize the application
- **Reverse Proxy**: Use Nginx for SSL and load balancing
- **Monitoring**: Implement health check endpoint
- **Data Persistence**: Optionally save in-memory data to JSON files periodically
- **Graceful Shutdown**: Save data before server stops

---

## Summary

This document provides a complete blueprint for implementing the TrainTrack backend in C++ using OOP principles. The key components are:

1. **RESTful API** with clear endpoints for all frontend operations
2. **In-memory storage** using STL containers (no database required)
3. **OOP class structure** following MVC pattern
4. **Authentication** using JWT tokens
5. **Thread-safe operations** for concurrent access
6. **Comprehensive error handling**
7. **Recommended libraries** for rapid development

The backend should be implemented in phases:
1. Core infrastructure (HTTP server, router, in-memory storage)
2. Data structures and initialization
3. Authentication system
4. Train search functionality
5. Booking system
6. Testing and refinement

---

**Next Steps**: Refer to `INTEGRATION_GUIDE.md` for instructions on connecting the frontend to this backend.
