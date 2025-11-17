Train-Ticket-Booking
A modern, full-featured Railway Ticket Booking web application — TrainTrack.
It combines a responsive, elegant HTML/Tailwind CSS frontend with a high-performance C++ REST API backend.

Project Overview
Train-Ticket-Booking allows users to:

Search for trains between stations across India

Instantly book tickets with auto seat allocation

Manage their profile and bookings

Enjoy secure, simulated payments and booking confirmations

Use an AI-powered trip planner for travel inspiration

Leverage a clean, mobile-first UI built with Tailwind CSS

Features
Frontend
Responsive, single-page UX with Tailwind CSS for rapid UI development

Hero search form for quick route queries (index.html)

Features and benefits section (instant booking, secure payments, AI trip planning)

Booking flow: search → choose train → add passengers → pay → confirmation page

Profile page for managing user info and booking history

Backend (/backend)
C++ REST API Server with JSON responses

User authentication (register, login, profile update) with token authorization

Train search with route-based filtering

Seat and booking management with automatic allocation

In-memory data storage (using STL containers)

Thread-safe concurrency

RESTful API with CORS support for UI integration

Docker and Docker Compose setup for easy local deployment

Quick Start
Prerequisites
Docker

Docker Compose

Modern browser

Running Locally (Backend)
bash
cd backend
docker-compose up -d
# To view logs:
docker-compose logs -f
# To stop the server:
docker-compose down
The backend runs at: http://localhost:18080

API Endpoints
Authentication
POST /api/auth/register — Register a new user

POST /api/auth/login — Login and receive token

GET /api/auth/profile — Get profile (requires Auth)

PUT /api/auth/profile — Update profile (requires Auth)

Search
GET /api/search?from=<FROM>&to=<TO> — Find available trains

Booking
POST /api/bookings — Book tickets (requires Auth)

GET /api/bookings — User’s bookings (requires Auth)

GET /api/bookings/:bookingId — Booking details

DELETE /api/bookings/:bookingId — Cancel booking

→ See backend/API_ROUTES.md for detailed usage and examples

Frontend (index.html)
Open index.html in your browser for the full booking UI

Directory Structure
text
Train-Ticket-Booking/
├── index.html                # Frontend UI
├── backend/
│   ├── src/                  # C++ REST API source
│   ├── include/              # Header files
│   ├── data/                 # In-memory/test data
│   ├── API_ROUTES.md         # API endpoint guide
│   ├── Dockerfile, docker-compose.yml
│   ├── CMakeLists.txt
│   ├── config.json           # Server config
│   └── README.md             # Backend README
Technologies
Frontend: HTML, Tailwind CSS, JavaScript

Backend: Modern C++, STL, REST API (JSON), Docker

DevOps: Docker, Docker Compose

License
Created for educational purposes.

Acknowledgements
Inspired by IRCTC-like applications and designed with clean code/OOP principles in C++.
Feel free to suggest enhancements or report issues!
