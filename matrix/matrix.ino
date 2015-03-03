#include <StandardCplusplus.h>
#include <vector>
#include <Char2Array.h>
#include <Arduino.h>
#include "EEPROM.h"
#include "DB.h"


Char2Array converter;



const int data = 4;
const int clock = 5;
const int latch = 6;
const int blank = 7;

const int button_pin = 3;
const int led_pin = 12;

const int total_shifts = 6;
int shift_count = 0;

void shift(){
  if(shift_count > total_shifts - 2){
    digitalWrite(data, HIGH);
    digitalWrite(clock, HIGH);
    digitalWrite(data, LOW);
    digitalWrite(clock, LOW);
    
    shift_count = 0;
  } 
  else{
    digitalWrite(clock, HIGH);
    digitalWrite(clock, LOW);
    
    shift_count += 1;
  }
  
  digitalWrite(latch, HIGH);
  digitalWrite(latch, LOW);
  
}

//Gets the next complete input from serial
//All user input must end with >, known as "end of message" character
//If multiple use inputs are made, it will collect both
String wait_for_next(){
    char temp_byte;
    String temp = "";
    //Waits until user inputs
    while(Serial.available() == 0){}
    //Loops until the end of message character is seen (>)
    while(Serial.peek() != '>'){
        temp_byte = Serial.read();
        temp += String(temp_byte);
        //Waits until next character is available in stream
        while(Serial.available() == 0){}
    }
    //Don't know why this works, sorta clears the bufferish??? Just don't touch it.
    //Seriously don't remove it
    //Not joking
    while(Serial.available() > 0){Serial.read();}
    //Whew made it to the other side
    
    return temp;
}



void admin_mode(){
  Serial.begin(9600);
  
   while(Serial.available() == 0){
       Serial.println("Welcome to admin mode. Enter any input to begin. Remember to end all input with '>'.");
       digitalWrite(led_pin, HIGH);
       delay(150);
       digitalWrite(led_pin, LOW);
       delay(150);
       digitalWrite(led_pin, HIGH);
       delay(750);
       digitalWrite(led_pin, LOW);
   } 
   digitalWrite(led_pin, HIGH); 
   while(wait_for_next() != "exit"){
     Serial.println("Enter 'exit' to exit");}
     digitalWrite(led_pin, LOW);
     
  Serial.end();
}

void check_for_button(){
    boolean pressed = false;
    unsigned long start = millis();
    digitalWrite(led_pin, HIGH);
    while(millis() - start < 10000){
        if(digitalRead(button_pin)){pressed = true; break;} 
        digitalWrite(led_pin, HIGH);
        delay(500);
        if(digitalRead(button_pin)){pressed = true; break;} 
        digitalWrite(led_pin, LOW);
        delay(500);
    }
    digitalWrite(led_pin, LOW);
    if(pressed){admin_mode();}
}

void setup(){
   pinMode(led_pin, OUTPUT);
   pinMode(button_pin, INPUT);
   
   pinMode(latch, OUTPUT);
   pinMode(data, OUTPUT);
   pinMode(clock, OUTPUT);
   pinMode(blank, OUTPUT);
   
   //check_for_button();
 }


void loop(){
   shift();
   delay(250);
   
   
  
}
