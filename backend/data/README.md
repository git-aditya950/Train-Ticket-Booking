# Train Booking System - Fake Data Documentation

## Overview
This directory contains fake/sample data used for testing and demonstration of the Train Booking System backend. The data is designed to be realistic and comprehensive, covering major train routes across India.

## Data Structure

### Trains Database
The system initializes with **100+ trains** covering routes across India, including:

#### Train Categories
1. **Rajdhani Express** (Premium trains with AC classes only)
   - Examples: Mumbai Rajdhani (12951), Howrah Rajdhani (12301), Bengaluru Rajdhani (22691)
   - Classes: 3A, 2A, 1A
   - Price Range: ₹2,500 - ₹7,100

2. **Shatabdi Express** (Day trains with chair car seating)
   - Examples: New Delhi-Bhopal Shatabdi (12001), Kalka Shatabdi (12011)
   - Classes: CC (Chair Car), EC (Executive Chair Car)
   - Price Range: ₹700 - ₹3,200

3. **Duronto Express** (Non-stop long-distance trains)
   - Examples: Howrah Duronto (12273), Pune Duronto (12263)
   - Classes: SL, 3A, 2A
   - Price Range: ₹1,000 - ₹4,000

4. **Mail/Express Trains** (Regular long-distance trains)
   - Examples: Punjab Mail (12137), Kerala Express (12625), Golden Temple Mail (12903)
   - Classes: SL, 3A, 2A, sometimes 1A
   - Price Range: ₹400 - ₹5,200

5. **Sampark Kranti/Intercity** (Regional connectivity trains)
   - Examples: Bihar Sampark Kranti (12565), Vikramshila Express (12367)
   - Classes: SL, 3A, 2A, 2S
   - Price Range: ₹250 - ₹2,700

### Station Coverage
The system includes **67 major stations** across India:

#### Major Metros
- New Delhi (NDLS)
- Mumbai (CST)
- Kolkata (HWH)
- Chennai (MAS)
- Bangalore (SBC)
- Hyderabad (SC)

#### Regional Hubs
- Pune, Ahmedabad, Jaipur, Lucknow, Bhopal, Indore, Patna
- Guwahati, Thiruvananthapuram, Kochi, Goa
- Amritsar, Chandigarh, Jammu, Varanasi

#### Tourist Destinations
- Shimla, Dehradun, Haridwar, Agra, Udaipur, Jodhpur
- Mysore, Puri, Tirupati, Kanyakumari, Rameswaram

### Class Types
- **1A (First AC)**: Premium air-conditioned compartments
- **2A (Second AC)**: Air-conditioned two-tier sleeper
- **3A (Third AC)**: Air-conditioned three-tier sleeper
- **SL (Sleeper)**: Non-AC sleeper class
- **CC (Chair Car)**: Air-conditioned seating
- **EC (Executive Chair Car)**: Premium chair car seating
- **2S (Second Seating)**: Non-AC seating class

## Availability Logic

### Seat Availability
The system generates realistic availability using randomization:

- **Available (AVL)**: Positive numbers (e.g., "AVL 50" means 50 seats available)
- **Waitlist (WL)**: Negative numbers (e.g., "WL 20" means 20 on waitlist)
- **RAC (Reservation Against Cancellation)**: Special status between confirmed and waitlist

### Seat Capacity by Class
- **1A**: 24 seats per coach
- **2A**: 48 seats per coach
- **3A**: 64 seats per coach
- **SL**: 200+ seats (multiple coaches)
- **CC**: 80 seats
- **EC**: 40 seats
- **2S**: 200+ seats

### Dynamic Pricing
Prices vary based on:
- Train type (Rajdhani > Shatabdi > Duronto > Mail)
- Distance traveled
- Class of travel
- Randomization for realism (±10-20% variation)

## Data Initialization

### Automatic Generation
The `DataStore::initializeSampleData()` method automatically:
1. Creates 100+ train records with bidirectional routes
2. Assigns appropriate classes based on train type
3. Generates random but realistic availability (50-300 seats depending on class)
4. Sets varied pricing (₹250 to ₹7,100)
5. Indexes trains by route for fast searching

### Route Indexing
Trains are indexed by route combinations:
- Format: "FromStation|ToStation"
- Example: "New Delhi (NDLS)|Mumbai (CST)"
- Enables fast O(1) lookups for train searches

## Usage Examples

### Search Trains
```cpp
// Search trains from Delhi to Mumbai
std::vector<Train> trains = dataStore->searchTrains(
    "New Delhi (NDLS)", 
    "Mumbai (CST)"
);
```

### Get Train Details
```cpp
// Find specific train
Train* train = dataStore->findTrainByNumber("12951");
if (train) {
    std::cout << train->getTrainName() << std::endl;
    std::cout << "From: " << train->getFromStation() << std::endl;
    std::cout << "To: " << train->getToStation() << std::endl;
}
```

### Check Availability
```cpp
// Check 3A class availability
int available = train->getAvailableSeats("3A");
double price = train->getPrice("3A");
```

### Book Tickets
```cpp
// Book 2 tickets in 3A class
bool success = train->updateAvailability("3A", 2);
if (success) {
    std::cout << "Booking confirmed!" << std::endl;
}
```

## Realistic Features

### 1. Bidirectional Routes
Every train has both up and down directions:
- 12951: Mumbai → Delhi
- 12952: Delhi → Mumbai

### 2. Journey Duration
Realistic travel times calculated based on distance:
- Delhi → Mumbai: ~15-16 hours
- Delhi → Kolkata: ~17 hours
- Delhi → Chennai: ~35-40 hours

### 3. Departure/Arrival Times
Typical patterns followed:
- Morning departures: 06:00 - 08:00
- Evening departures: 17:00 - 20:00
- Overnight trains: 23:00 - 02:00

### 4. Train Schedules
Some trains run:
- Daily (Mon-Sun)
- Selected days (Mon, Wed, Fri)
- Weekend only (Sat, Sun)

### 5. Waitlist Simulation
Some popular routes show waitlist status (negative availability) to simulate high demand periods.

## Testing Scenarios

### High Availability Routes
- Patna → New Delhi (Bihar Sampark Kranti): 300+ SL seats
- Kolkata → Chennai (Coromandel Express): 110+ SL seats

### Limited Availability Routes
- Mumbai → Delhi (Mumbai Rajdhani): Only 2-8 1A seats
- Delhi → Goa (Goa Express): Limited 2A seats

### Waitlist Routes
- Mumbai → Amritsar (Punjab Mail): SL class waitlist
- Mumbai → Amritsar (Golden Temple Mail): SL RAC/WL

## Data Files

### trains.json
Contains structured train data including:
- Train numbers and names
- Route information
- Timings and duration
- Running days
- Class-wise availability and pricing
- Station lists
- Class type descriptions

## Future Enhancements

1. **Date-based Availability**: Different availability for different journey dates
2. **Seasonal Pricing**: Higher prices during peak seasons
3. **Dynamic Quota**: Tatkal, Premium Tatkal, Ladies quota
4. **Train Status**: Running status, delays, cancellations
5. **Route Stations**: Intermediate station stops
6. **Fare Rules**: Cancellation charges, refund policies

## Notes

- All data is **fictional** and for **testing purposes only**
- Station codes follow actual Indian Railways conventions
- Pricing is approximate and may not reflect actual fares
- Availability is randomly generated on each system initialization
- The system uses `rand()` for variation, so each run produces slightly different availability

## Contact

For questions or modifications to the fake data structure, refer to:
- `include/models/Train.h` - Train model definition
- `src/utils/DataStore.cpp` - Data initialization logic
- `include/utils/DataStore.h` - Data storage interface
