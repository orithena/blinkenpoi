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
  state.running++;
  if(state.running>total_animations) state.running=0;
  load_animation(state.running);
} 


//TODO set flag and handle content elsewhere 
void longpress2() {
  Serial.println("Button 2 longpress.");
  reset_config = true;
}
