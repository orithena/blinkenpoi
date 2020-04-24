boolean checkButtons()
{

  button2.tick();
//  button2.tick();

  if(!digitalRead(button2_pin)) return true;
 
  return false;
  
}




//TODO set flag and handle content elsewhere
void click2() {
  Serial.println("Button 2 click.");
  state.running = (state.running + 1) % total_animations;
  load_animation(state.running);
} 


//TODO set flag and handle content elsewhere 
void longpress2() {
  Serial.println("Button 2 longpress.");
  if(!looping) {
    reset_config = true;
  } else {
    autoanimation = true;
  }
}
