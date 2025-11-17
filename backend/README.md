# TrainTrack Backend - C++ REST API Server

A high-performance C++ backend server for the TrainTrack railway booking application. Built with Object-Oriented Programming principles and in-memory data storage.

## Features

- **RESTful API** with JSON responses
- **User Authentication** with token-based authorization
- **Train Search** with route-based filtering
- **Booking System** with automatic seat allocation
- **In-Memory Storage** using STL containers
- **Thread-Safe** operations with mutex locks
- **CORS Support** for frontend integration

## Quick Start with Docker

### Prerequisites
- Docker
- Docker Compose

### Running the Server

```bash
# Start the server
docker-compose up -d

# View logs
docker-compose logs -f

# Stop the server
docker-compose down
```

The server will start on `http://localhost:18080`

## API Endpoints

### Base URL
```
http://localhost:18080/api
```

### Authentication Endpoints

#### 1. Register User
```http
POST /api/auth/register
Content-Type: application/json

{
  "name": "John Doe",
  "email": "john@example.com",
  "password": "SecurePassword123",
  "phone": "+91 98765 43210"
}
```

**Response (201):**
```json
{
  "status": "success",
  "message": "User registered successfully",
  "data": {
    "userId": "user_123456",
    "name": "John Doe",
    "email": "john@example.com",
    "phone": "+91 98765 43210",
    "createdAt": "2025-11-17T10:30:00.000Z"
  },
  "token": "token_abc123..."
}
```

#### 2. Login
```http
POST /api/auth/login
Content-Type: application/json

{
  "email": "john@example.com",
  "password": "SecurePassword123"
}
```

**Response (200):**
```json
{
  "status": "success",
  "message": "Login successful",
  "data": {
    "userId": "user_123456",
    "name": "John Doe",
    "email": "john@example.com",
    "phone": "+91 98765 43210"
  },
  "token": "token_abc123..."
}
```

#### 3. Get Profile (Protected)
```http
GET /api/auth/profile
Authorization: Bearer <token>
```

#### 4. Update Profile (Protected)
```http
PUT /api/auth/profile
Authorization: Bearer <token>
Content-Type: application/json

{
  "name": "John Smith",
  "phone": "+91 99999 99999",
  "currentPassword": "OldPassword123",
  "newPassword": "NewPassword456"
}
```

### Train Search Endpoint

#### Search Trains
```http
GET /api/search?from=New%20Delhi%20(NDLS)&to=Mumbai%20(CST)
```

**Response (200):**
```json
{
  "status": "success",
  "count": 1,
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
        }
      ]
    }
  ]
}
```

### Booking Endpoints

#### 1. Create Booking (Protected)
```http
POST /api/bookings
Authorization: Bearer <token>
Content-Type: application/json

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
    }
  ]
}
```

**Response (201):**
```json
{
  "status": "success",
  "message": "Booking confirmed successfully",
  "data": {
    "bookingId": "booking_789012",
    "pnr": "654-3219870",
    "userId": "user_123456",
    "train": { ... },
    "selectedClass": { ... },
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
}
```

#### 2. Get All Bookings (Protected)
```http
GET /api/bookings
Authorization: Bearer <token>
```

#### 3. Get Booking by ID (Protected)
```http
GET /api/bookings/:bookingId
Authorization: Bearer <token>
```

#### 4. Cancel Booking (Protected)
```http
DELETE /api/bookings/:bookingId
Authorization: Bearer <token>
```

## Testing the API

Test the endpoints using curl, Postman, or any HTTP client. Examples:

## Configuration

Edit `config.json` to customize server settings.

## License

This project is created for educational purposes.
