/*
 * mercury.c - Mercury: A Spark Core Powered Smart Mailbox
 * Copyright (c) 2014 Carter Yagemann & Ross Lazerowitz
 *
 * This C file is the main source code file for the Mercury Smart Mailbox.
 * This code is intended to be ran on a Spark Core micro-controller attached
 * to three IR proximity sensors. These sensors are wired to analogue data pins
 * A0, A1, and A2.
 *
 * This code is licensed under the MIT License (MIT).
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "HttpClient/HttpClient.h" // Library for making HTTP requests
#include <math.h>                  // abs

/***************************************
 **          HTTP CONFIG              **
 ***************************************/
HttpClient http;
// HTTP header
http_header_t headers[] = {
    { "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headers will NULL
};
http_request_t request;
http_response_t response;

/***************************************
 **             CONFIG                **
 ***************************************/
// Pins for proximity sensors
const int ps1 = A0;
const int ps2 = A1;
const int ps3 = A2;
// Pin for built in LED
int led2 = D7;
// Delay of sensors in milliseconds
const int delayTime = 20;
// Readings per average
const int readings = 20;
// Alpha needed to trigger mailbox
const double alpha = 0.10;
// Holders for readings
int currentValue1, currentValue2, currentValue3;
int oldValue1, oldValue2, oldValue3;
double diff1, diff2, diff3;


/***************************************
 **             METHODS               **
 ***************************************/
 
/*
 * Helper function for what to do when the mailbox
 * is triggered.
 */
void triggerMailbox() {
  // This server sends an email when it receives a request
  // It is very simple and could be further developed
  request.ip = {127,0,0,1};
  request.port = 80;
  request.path = "/";

  // Make HTTP GET request to server
  http.get(request, response, headers);

  // Record result to spark core's network API
  Spark.variable("status", &response.status, STRING);
  Spark.variable("body", &response.body, STRING);

  // Turn on LED for 5 seconds
  digitalWrite(led2, HIGH);
  delay(5000);
  digitalWrite(led2, LOW);
}

/*
 * Prepares and calibrates spark core upon boot.
 */
void setup() {

  // Set built in LED pin
  pinMode(led2, OUTPUT);
  
  // Set pins into input mode
  pinMode(ps1, INPUT);
  pinMode(ps2, INPUT);
  pinMode(ps3, INPUT);
  
  // Calibrate proximity sensors
  for (int i = 0; i < readings; i++) {
    oldValue1 += analogRead(ps1);
    oldValue2 += analogRead(ps2);
    oldValue3 += analogRead(ps3);
    delay(delayTime);
  }
  
  // Take an average of our readings to smooth them out
  oldValue1 /= readings;
  oldValue2 /= readings;
  oldValue3 /= readings;
}

/*
 * Main loop.
 *
 * Takes multiple readings from proximity sensors, averages them
 * out and compares the average against the calibrated average.
 * If the difference in readings is greater than alpha, the mailbox
 * is triggered.
 */
void loop() {
  // Zero integers in preparation for reading
  currentValue1 = currentValue2 = currentValue3 = 0;
  
  // Take readings
  for (int i = 0; i < readings; i++) {
    currentValue1 += analogRead(ps1);
    currentValue2 += analogRead(ps2);
    currentValue3 += analogRead(ps3);
    delay(delayTime);
  }
  
  // Take average
  currentValue1 /= readings;
  currentValue2 /= readings;
  currentValue3 /= readings;
  
  // Calculate difference
  diff1 = abs((currentValue1 - oldValue1) / oldValue1);
  diff2 = abs((currentValue2 - oldValue2) / oldValue2);
  diff3 = abs((currentValue3 - oldValue3) / oldValue3);
  
  // If change was significant, trigger mailbox
  if (diff1 > alpha || diff2 > alpha || diff3 > alpha) {
    triggerMailbox();
  }
}