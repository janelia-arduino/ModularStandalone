// ----------------------------------------------------------------------------
// Server.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "Server.h"


namespace Standalone
{
int Server::interactive_variable_count_ = 0;
volatile int Server::interactive_variable_index_ = -1;
volatile boolean Server::interactive_variable_index_changed_ = false;

Server::Server(HardwareSerial &serial,
               const int enc_a_pin,
               const int enc_b_pin,
               const int enc_btn_pin,
               const int enc_btn_int,
               const int btn_pin,
               const int btn_int,
               const int led_pwr_pin,
               const int update_period) :
  display_(serial),
  encoder_(enc_b_pin,enc_a_pin),
  enc_btn_pin_(enc_btn_pin),
  enc_btn_int_(enc_btn_int),
  btn_pin_(btn_pin),
  btn_int_(btn_int),
  led_pwr_pin_(led_pwr_pin),
  update_period_(update_period)
{
}

void Server::setup()
{
  display_.setup();
  disable();
  time_last_update_ = millis();

  pinMode(enc_btn_pin_,INPUT);
  digitalWrite(enc_btn_pin_,HIGH);
  attachInterrupt(enc_btn_int_,encBtnIsr,FALLING);

  pinMode(led_pwr_pin_,INPUT);

  display_labels_dirty_ = true;
}

void Server::enable()
{
  enabled_ = true;
}

void Server::disable()
{
  enabled_ = false;
  ledOff();
  display_.displayOff();
}

void Server::update()
{
  if (!enabled_)
  {
    return;
  }

  if (led_off_ && (digitalRead(led_pwr_pin_) == HIGH))
  {
    ledOn();
  }
  else if (!led_off_ && (digitalRead(led_pwr_pin_) == LOW))
  {
    ledOff();
  }

  unsigned long time_now = millis();
  if ((time_now - time_last_update_) < update_period_)
  {
    return;
  }

  time_last_update_ = time_now;
  // Update current interactive variable values
  if (Server::interactive_variable_index_ >= 0)
  {
    InteractiveVariable &int_var = interactive_variable_vector_[Server::interactive_variable_index_];
    if (!int_var.checkValueDirty() && !Server::interactive_variable_index_changed_)
    {
      int_var.updateWithEncoderValue(encoder_.read());
    }
    // updateWithEncoderValue may have dirtied value
    if (int_var.checkValueDirty() || Server::interactive_variable_index_changed_)
    {
      encoder_.write(int_var.getValue());
      int_var.clearValueDirty();
      Server::interactive_variable_index_changed_ = false;
    }
  }
  // Update all display_labels on display if necessary
  if (display_labels_dirty_)
  {
    display_labels_dirty_ = false;
    for (std::vector<DisplayLabel>::iterator display_label_it = display_label_vector_.begin();
         display_label_it != display_label_vector_.end();
         ++display_label_it)
    {
      display_label_it->updateOnDisplay(display_);
    }
  }
  // Update all display_variables on display
  for (std::vector<DisplayVariable>::iterator display_var_it = display_variable_vector_.begin();
       display_var_it != display_variable_vector_.end();
       ++display_var_it)
  {
    display_var_it->updateOnDisplay(display_);
  }
  // Update all interactive_variables on display
  for (std::vector<InteractiveVariable>::iterator int_var_it = interactive_variable_vector_.begin();
       int_var_it != interactive_variable_vector_.end();
       ++int_var_it)
  {
    int_var_it->updateOnDisplay(display_);
  }
  // Place the cursor back on the current interactive variable
  if (Server::interactive_variable_index_ >= 0)
  {
    InteractiveVariable &int_var = interactive_variable_vector_[Server::interactive_variable_index_];
    display_.setCursor(int_var.getDisplayPosition());
  }
}

DisplayLabel& Server::createDisplayLabel()
{
  DisplayLabel display_label;
  display_label_vector_.push_back(display_label);
  return display_label_vector_.back();
}

DisplayVariable& Server::createDisplayVariable()
{
  DisplayVariable display_var;
  display_variable_vector_.push_back(display_var);
  return display_variable_vector_.back();
}

InteractiveVariable& Server::createInteractiveVariable()
{
  InteractiveVariable int_var;
  interactive_variable_vector_.push_back(int_var);
  if (Server::interactive_variable_index_ < 0)
  {
    Server::interactive_variable_index_ = 0;
    display_.blinkingCursorOn();
  }
  Server::interactive_variable_count_++;
  return interactive_variable_vector_.back();
}

void Server::encBtnIsr()
{
  if (Server::interactive_variable_index_ >= 0)
  {
    int int_var_prev = Server::interactive_variable_index_;
    Server::interactive_variable_index_++;
    if (Server::interactive_variable_index_ >= Server::interactive_variable_count_)
    {
      Server::interactive_variable_index_ = 0;
    }
    if (int_var_prev != Server::interactive_variable_index_)
    {
      Server::interactive_variable_index_changed_ = true;
    }
  }
}

void Server::ledOn()
{
  display_.displayOn();
  display_.setBrightnessDefault();
  led_off_ = false;
}

void Server::ledOff()
{
  display_.setBrightness(0);
  led_off_ = true;
}
}
