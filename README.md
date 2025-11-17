# TrainTrack — Train-Ticket-Booking

**TrainTrack** is a modern, educational Railway Ticket Booking web application that combines a responsive HTML/Tailwind CSS frontend with a high-performance C++ REST API backend. This README gives a clear overview, setup instructions, API summary, and developer notes so anyone can run, test, and contribute to the project.

---

## Table of Contents

* [Project Overview](#project-overview)
* [Features](#features)
* [Demo / Screenshots](#demo--screenshots)
* [Tech Stack](#tech-stack)
* [Repository Structure](#repository-structure)
* [Quick Start (Docker)](#quick-start-docker)

  * [Backend (Docker Compose)](#backend-docker-compose)
  * [Frontend (index.html)](#frontend-indexhtml)
* [API Endpoints (summary)](#api-endpoints-summary)
* [Development & Running Locally (non-container)](#development--running-locally-non-container)
* [Testing](#testing)
* [Suggested Improvements](#suggested-improvements)
* [Contributing](#contributing)
* [License](#license)
* [Acknowledgements](#acknowledgements)

---

## Project Overview

TrainTrack is designed as an IRCTC-like educational project to demonstrate a full-stack pattern where a static, mobile-first frontend communicates with a C++ REST API backend. The application supports searching trains between stations, booking tickets with automatic seat allocation, user authentication, booking management, and simulated payments.

The project is useful as a learning reference for:

* Building REST APIs in modern C++ (JSON responses, routing, concurrency)
* Simple in-memory data models and thread-safe handling using STL and synchronization primitives
* Integrating a static frontend (HTML + Tailwind + JavaScript) with a REST backend
* Containerizing services using Docker and Docker Compose

---

## Features

* Train search by origin & destination
* Booking flow: search → select train → add passenger(s) → simulated payment → confirmation
* User authentication (register/login) with token-based access for protected routes
* Booking history and booking cancellation
* In-memory seat allocation and booking management
* Docker + docker-compose support for straightforward local deployment

---

## Demo / Screenshots

Open `index.html` in a modern browser to view the frontend UI and manually exercise the booking flow. Consider running the backend (see Quick Start) to enable end-to-end functionality.

---

## Tech Stack

* Frontend: HTML, Tailwind CSS, JavaScript
* Backend: Modern C++ (STL), REST API serving JSON
* DevOps: Docker, Docker Compose

---

## Repository Structure

```
Train-Ticket-Booking/
├── index.html               # Frontend UI (single-page)
├── README.md                # This file (add to repo root)
├── backend/
│   ├── src/                 # C++ source files
│   ├── include/             # Header files
│   ├── data/                # In-memory/test data (sample trains, users)
│   ├── API_ROUTES.md        # Detailed API routes & examples
│   ├── Dockerfile
│   ├── docker-compose.yml
│   ├── CMakeLists.txt
│   ├── config.json          # server configuration
│   └── README.md            # Backend-specific notes
└── ...
```

> The above structure mirrors the repository and helps new developers quickly find the frontend and backend pieces.

---

## Quick Start (Docker)

**Prerequisites**: Docker and Docker Compose installed, modern browser.

### Backend (Docker Compose)

1. Open a terminal and navigate to the repository root.

2. Start the backend:

```bash
cd backend
docker-compose up -d
```

3. View logs (optional):

```bash
docker-compose logs -f
```

4. Stop the server:

```bash
docker-compose down
```

The backend defaults to running at `http://localhost:18080` (see `config.json` or `docker-compose.yml` to confirm).

### Frontend (index.html)

* Open `index.html` in your browser directly (double-click or `file://`), or serve it via a local static server for features requiring CORS.

**Note:** For full functionality (bookings, profile, payments) the frontend needs the backend running since those actions call the REST API.

---

## API Endpoints (summary)

> See `backend/API_ROUTES.md` for detailed examples and payload shapes. The quick summary below lists the main endpoints provided by the backend.

### Authentication

* `POST /api/auth/register` — Register new user
* `POST /api/auth/login` — Login and receive token
* `GET  /api/auth/profile` — Get user profile (requires token)
* `PUT  /api/auth/profile` — Update profile (requires token)

### Search

* `GET /api/search?from=<station>&to=<station>` — Search for trains between stations

### Bookings

* `POST   /api/bookings` — Create a booking (requires token)
* `GET    /api/bookings` — List user bookings (requires token)
* `GET    /api/bookings/:bookingId` — Booking details
* `DELETE /api/bookings/:bookingId` — Cancel a booking

### Notes

* API uses JSON for requests and responses.
* Authentication is token-based (send token in `Authorization` header).
* See `backend/API_ROUTES.md` for sample requests, sample responses, and error formats.

---

## Development & Running Locally (non-container)

If you prefer to build and run the backend locally without Docker:

1. Ensure you have a modern C++ toolchain (C++17 or later), CMake, and any libraries the project uses.
2. From `backend/` run:

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
./traintrack_server --config ../config.json
```

3. Open `index.html` and configure the frontend to point to `http://localhost:18080` if needed.

**Tip:** Check the backend `README.md` inside `backend/` for any project-specific build flags or dependencies.

---

## Testing

* Manual testing: Use the frontend UI and exercise the booking, login, and cancellation flows.
* API testing: Use `curl`, Postman, or HTTPie to call endpoints described in `backend/API_ROUTES.md`.

Example `curl` (search):

```bash
curl "http://localhost:18080/api/search?from=Mumbai&to=Delhi"
```

---

## Suggested Improvements

If you want to expand the project, here are suggested next steps:

* Persist data to a real database (SQLite/Postgres) instead of in-memory storage
* Add proper password hashing & secure token management
* Add unit & integration tests for the backend
* Implement payment gateway integration (sandboxed)
* Add frontend routing and nicer UX for mobile
* Add CI (GitHub Actions) to build and run tests on pull requests

---

## Contributing

Contributions are welcome! Please open an issue first for larger features. For small fixes, fork the repo, create a feature branch, and open a pull request.

A simple contribution workflow:

1. Fork the repository
2. Create a feature branch: `git checkout -b feat/my-feature`
3. Commit your changes and push
4. Open a pull request describing the change

---

## License

This project is created for educational purposes. Add a LICENSE file if you want to apply an open-source license (MIT/Apache-2.0/etc.).

---

## Acknowledgements

Inspired by IRCTC-like ticketing apps and built with clean-code and OOP design in C++.

---

*If you'd like, I can also:*

* Create a `README.md` file and open a PR to your repository (I can provide the exact commit message & diff to apply).
* Shorten or expand any section above, or convert parts into separate docs (e.g., `CONTRIBUTING.md`, `DEVELOPER.md`).
