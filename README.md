# IoT-ESP32

### A production-ready multi-threaded HTTP server written in C that accepts event data via REST API, generates professional PDF reports, and delivers them via email with attachments.

## 🎯 Features

- **Multi-threaded Architecture**: Thread pool with 8 worker threads handling concurrent requests
- **HTTP/1.1 Server**: Custom HTTP parser with RESTful API endpoint (`POST /submit`)
- **PDF Report Generation**: Automated report creation using libharu with professional formatting
- **Email Delivery**: SMTP integration via libcurl with MIME multipart and Base64-encoded attachments
- **Scheduled Reports**: Background thread generates and emails reports at configurable intervals
- **Thread-Safe Operations**: Mutex and condition variable synchronization for safe concurrent access

## 🛠️ Tech Stack

- **Language**: C
- **Concurrency**: POSIX Threads (pthread)
- **Libraries**: 
  - libharu - PDF generation
  - libcurl - SMTP/email
  - cJSON - JSON parsing
- **Architecture**: Producer-consumer pattern with thread pool

## 📐 Architecture

- **Main Thread**: Accepts TCP connections and enqueues to worker pool
- **Worker Threads**: Parse HTTP requests, validate JSON, write events to PDF
- **Scheduler Thread**: Periodically saves PDF and emails report to recipients
- **Thread Synchronization**: Mutex-protected shared resources with condition variables for efficient signaling

## 🚀 How It Works

1. Client sends HTTP POST request with JSON event data (event type, timestamp, recipient email)
2. Worker thread validates JSON and appends event to active PDF report
3. After configured interval (default: 60 seconds), scheduler thread:
   - Finalizes current PDF report
   - Sends email with PDF attachment via SMTP
   - Initializes new PDF for next batch of events
4. Multiple clients can submit events concurrently - thread pool handles parallelism

## 💻 Installation

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get install build-essential libhpdf-dev libcurl4-openssl-dev

# macOS
brew install libharu curl
```

### Build
```bash
git clone https://github.com/shlomoPearl/IoT-ESP32.git
cd IoT-ESP32
make
```

### Test
```bash
make run
chmod +x test.sh
./test.sh

# Check the console output for success messages and email delivery confirmation.
```

### Configuration
Set environment variables for email:
```bash
export MAIL_S="your_email@gmail.com"
export MAIL_PASS="your_app_password"  # Gmail App Password
```

For Gmail: Enable 2FA and generate App Password at https://myaccount.google.com/apppasswords

## 📡 API Usage

### Submit Event
```bash
curl -X POST http://localhost:7777/submit \
  -H "Content-Type: application/json" \
  -d '{
    "EMAIL": "recipient@example.com",
    "EVENT": "motion_detected",
    "TIME": "2024-12-30 10:15:23"
  }'
```

**Response:**
```json
{"message":"Success"}
```

## 🧵 Threading Model

- **Producer-Consumer Pattern**: Main thread produces client connections, workers consume
- **Thread-Safe Queue**: Mutex-protected FIFO queue with condition variable signaling
- **No Busy Waiting**: Threads sleep on condition variables until work available
- **Graceful Concurrency**: Mutex ensures safe access to shared PDF document


## 🔒 Security

- Email credentials from environment variables (never hardcoded)
- Input validation on all JSON fields
- Bounded buffer sizes prevent overflow attacks
- TLS/SSL for SMTP connections

## 🎓 What I Learned

- **Systems Programming**: Deep understanding of POSIX threads, mutex/condition variables, and concurrent data structures
- **Network Programming**: Socket programming, TCP/IP, HTTP protocol implementation
- **Library Integration**: Working with C libraries (libharu, libcurl) and managing dependencies
- **Memory Management**: Careful malloc/free patterns, valgrind for leak detection
- **Problem Solving**: Debugging race conditions, ensuring thread safety, handling edge cases

## 📄 License
This project is licensed under the MIT [LICENSE](./LICENSE).