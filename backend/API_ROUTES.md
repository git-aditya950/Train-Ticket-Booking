# API Routes Documentation

## Authentication Routes

### POST /api/auth/register
**Description:** Register a new user account  
**Authentication:** Not required  
**Input:**
- name (required) - User's full name
- email (required) - User's email address
- password (required) - Password (minimum 8 characters)
- phone (optional) - User's phone number

**Output:**
- User profile information
- Authentication token
- Success message

### POST /api/auth/login
**Description:** Login to an existing account  
**Authentication:** Not required  
**Input:**
- email (required) - User's email address
- password (required) - User's password

**Output:**
- User profile information
- Authentication token
- Success message

### POST /api/auth/logout
**Description:** Logout from current session  
**Authentication:** Required (Bearer token)  
**Input:**
- Authorization header with Bearer token

**Output:**
- Success message

### GET /api/auth/profile
**Description:** Get current user's profile  
**Authentication:** Required (Bearer token)  
**Input:**
- Authorization header with Bearer token

**Output:**
- User profile information including name, email, phone

### PUT /api/auth/profile
**Description:** Update current user's profile  
**Authentication:** Required (Bearer token)  
**Input:**
- Authorization header with Bearer token
- Updated profile fields (name, phone, password, etc.)
- Current password for verification (if changing password)

**Output:**
- Updated user profile information
- Success message

## Train Routes

### GET /api/search
**Description:** Search for trains between two stations  
**Authentication:** Not required  
**Input (Query Parameters):**
- from (required) - Source station name
- to (required) - Destination station name
- date (optional) - Journey date

**Output:**
- List of available trains with details
- Train number, name, departure/arrival times
- Class options and seat availability
- Fare information

## Booking Routes

### POST /api/bookings
**Description:** Create a new train booking  
**Authentication:** Required (Bearer token)  
**Input:**
- Authorization header with Bearer token
- train - Train details object
- selectedClass - Class information with class code
- journeyDate - Date of journey
- passengers - Array of passenger details (minimum 1, maximum 6)

**Output:**
- Booking confirmation details
- PNR number
- Booking ID
- Total fare and passenger information

### GET /api/bookings
**Description:** Get all bookings for the logged-in user  
**Authentication:** Required (Bearer token)  
**Input:**
- Authorization header with Bearer token
- status (optional query parameter) - Filter by booking status

**Output:**
- List of user's bookings
- Booking count
- Details for each booking including status, PNR, train info

### GET /api/bookings/:bookingId
**Description:** Get details of a specific booking  
**Authentication:** Required (Bearer token)  
**Input:**
- Authorization header with Bearer token
- bookingId (path parameter) - Unique booking identifier

**Output:**
- Complete booking details
- Train information
- Passenger list
- Status and fare information

### DELETE /api/bookings/:bookingId
**Description:** Cancel an existing booking  
**Authentication:** Required (Bearer token)  
**Input:**
- Authorization header with Bearer token
- bookingId (path parameter) - Unique booking identifier

**Output:**
- Cancellation confirmation
- Refund amount calculated based on cancellation policy
- Updated booking status

---

**Base URL:** http://localhost:18080  
**Port:** 18080  
**Content-Type:** application/json
