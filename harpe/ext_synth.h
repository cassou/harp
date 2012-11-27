/*void loop() {
  // play notes from F#-0 (0x1E) to F#-5 (0x5A):
  for (int note = 0x1E; note < 0x5A; note ++) {
    //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
    noteOn(0x90, note, 0x45);
    delay(100);
    //Note on channel 1 (0x90), some note value (note), silent velocity (0x00):
    noteOn(0x90, note, 0x00);   
    delay(100);
  }
}*/

//  plays a MIDI note.  Doesn't check to see that
//  cmd is greater than 127, or that data values are  less than 127:


//int noteSet[]{68,70,72,73,75,77,79,80,82,84};
//int noteSet[]{60,62,64,65,67,69,71,72,74,76};
//int noteSet[]{67,69,71,72,74,76,77,79,81,83}; //tetris ok
int noteSet[]{65,67,69,71,72,74,76,77,79,81,83}; //tetris ok


void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}
