Overview
========

Mercury is a smart mailbox which emails a user when mail is placed inside the mailbox. This project was 
made during the fall 2014 Hack Upstate hackathon and uses a Spark Core attached to three proximity sensors.

Setup
=====

This project required one Spark Core micro-controller and three IR proximity sensors. Optionally the Spark 
Core can also be given a battery supply to make it portable.

In the code example included, the sensors are assumed to be connected to A0, A1, and A2. We found the Spark 
Core's power to be enough to power the three sensors without need for an additional power source.

Areas of Improvement
====================

Since this was made during a 24 hour hackathon (and was actually finished in 12 hours) there are many possible 
areas of improvment:

1. The micro-controller resets after a few seconds meaning that it will continue sending emails until the mail 
is removed. Ideally, a state machine architecture should be implemented so it will not reset until the mail 
is removed.
2. The system was not optimized for energy usage. Ideally, a flip-flop design should be used to improve 
energy consumption.
3. Emails are sent by making a simple HTTP GET request to a web server. At the very least, this server's API 
should be expanded to require some authentication, but ideally the Spark Core could talk directly to popular 
email platforms so no web server would be needed.