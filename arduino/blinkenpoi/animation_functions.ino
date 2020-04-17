#define NUMPIXELS 50 // Number of LEDs in strip

Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);
//File animation_file;
//int animation_loaded=0;
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

int animate_from_file(struct AnimationState *state) {
  for(int i = 0; i < 25; i++ ) {
    state->pixels[i] = { r: getByte(), g: getByte(), b: getByte() };
  }
  return 0;
}

boolean load_animation(String filename)
{
  for( int a = 0; a < ARRAY_SIZE(animations); a++ ) {
    if( filename.substring(12).equals(animations[a].name) ) {
      state.callback = animations[a].func;
      state.loaded = 2;
      Serial.print("Loaded generative function ");
      Serial.print(filename.substring(12));
      return true;
    }
  }
  
  Serial.print("Trying to load: ");
  Serial.println(filename);
  state.file.close();
  state.file = SPIFFS.open(filename, "r"); 
  if (!state.file) {
    Serial.println("FAILED to open file :(");
    state.loaded=0; 
    return false;
  }
    
  state.file.seek(0); 
  state.loaded=1;
  state.callback = animate_from_file;
  return true;
}

boolean load_animation(int filenum)
{
  Serial.print("Trying to load anim #");
  Serial.println(filenum);
  state.file.close();
  if(state.running==0)
  {
    //Serial.println("animation_running=0");
    state.loaded=0;
    leds_all_off();
    return true;
  }

  int counter=0;
  if( state.running > 0 && state.running < 1+ARRAY_SIZE(animations) ) {
    return load_animation(animations[state.running-1].name);
  }
  #ifdef ESP8266
  Dir dir = SPIFFS.openDir("/animations/");
    while (dir.next()) {                      // List the file system contents
      counter++;
      String fileName = dir.fileName();
      if(counter==state.running) return load_animation(fileName.c_str());
    }
  #endif
  #ifdef ESP32
  File dir = SPIFFS.open("/animations");   // don't add a trailing slash!
  File file = dir.openNextFile();
    while (file) {                      // List the file system contents
      counter++;
      String fileName = file.name();
      Serial.printf("Checking #%d: %s\n", counter, fileName.c_str());
      if(counter==state.running) {
        return load_animation(fileName.c_str());
      }
      file.close();
      file = dir.openNextFile();
    }
    dir.close();
  #endif
}



uint8_t getByte()
{
  signed int data = state.file.read();
  
  if(data==-1) 
  {
    // reset file read position if we reached EOF
    state.file.seek(0);
    data = state.file.read();
  }
   
  return (uint8_t)data;
}

void showAnimation()
{
  if(state.loaded == 0) return;
  EVERY_N_MILLISECONDS(4) {
    // increase frame counter. 
    state.frame += 1;
  }

  int continue_this_animation = state.callback(&state);
  if( continue_this_animation < 0 ) {
    // animation said "STOP ME!!!"
    // well, we don't have any mode yet that implements autoswitching effects, so: we switch off.
    state.running = 0;
  } else
  if( continue_this_animation == 0 ) {
    // animation said "you may stop me. or repeat me. as you wish."
    // TODO: this is the point to implement autoswitching animations.
    copy_state_to_strip();
  } else
  if( continue_this_animation > 0 ) {
    // animation said "I need to continue for at least n frames"
    copy_state_to_strip();
  }
}

void copy_state_to_strip() {
      for(int i=0;i<25;i++)
    {
     int r= state.pixels[i].r;
     int g= state.pixels[i].g;
     int b= state.pixels[i].b;
  
      // anim data is flipped.. the javascript was too complicated at some point and i didnt care anymore :)
      strip.setPixelColor(24-i,r,g,b);
      // mirror on other side
      strip.setPixelColor(25+i,r,g,b);
    }
    strip.show();
}

void leds_all_off()
{
  for(int i=0;i<50;i++)
  {
    strip.setPixelColor(i,0,0,0);
  }
  strip.show();

}
