import g4p_controls.*;
import processing.serial.*;

Serial port;

void updateScoreboard(String update) {
  if (update.contains("scored!")) {
    String newScore = update.substring(27); // Cut off filler text, last numbers are score
    if (update.contains("Player 1")) {
      score1.setText(newScore);
    }
    if (update.contains("Player 2")) {
      score2.setText(newScore);
    }   
  }
  
  if (update.contains("Lost")) {
    if (update.contains("Player 1")) {
      score1.setText("X");
    }
    if (update.contains("Player 2")) {
      score2.setText("X");
    }    
  }
  
  if (update.contains("position")) {
    score1.setText("0");
    score2.setText("0");
  }
  
  if (update.contains("New high score")) {
    String newScore = update.substring(17); // Cut off filler text, last numbers are score
    highScore.setText(newScore);
  }
  
  if (update.contains("Starting in 3")) {
    countdown.setText("3");
  }
  if (update.contains("Starting in 2")) {
    countdown.setText("2");
  }
  if (update.contains("Starting in 1")) {
    countdown.setText("1");
  }
  if (update.contains("Starting Game")) {
    countdown.setText("");
  }
}

void setup() {

  String portName = Serial.list()[0]; // COM3
  port = new Serial(this, portName, 9600);
  
  size(960, 640, JAVA2D);
  createGUI();
  update = "";
}

String update;

void draw() {
  background(230);
  
  if (port.available() > 0) {
    update = port.readStringUntil('\n');
    if (update != null && update.length() > 0) {
      System.out.println(update);
      updateScoreboard(update);
    }
  }

}
