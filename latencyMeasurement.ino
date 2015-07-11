//int serialRate = 28800;
int serialRate = 9600;

boolean isSend = false;
boolean isRecieve = false;
boolean isEnded = false;

static int NUM_TRY = 10;

int counter = 0;
unsigned int total = 0;

unsigned long startTick[20];
unsigned long endTick[20];
unsigned long tick;

void setup() {
  Serial.begin(serialRate);
  tick = millis();
}

void loop() {
  //処理が終わってたらもうやらなーい
  if (!isEnded) {
    if (tick + 500 < millis()) {
      while(true) {
        //送ってない,受け取ってない=>送ろう
        if (!isRecieve && !isSend) {
          sendMessage();
        }

        //送った=>到着待ち
        if (Serial.available()) {
          recieveMessage();
        }

        //受け取った=>レイテンシ計算,フラグ初期化
        if (isRecieve) {
          //フラグ類の再初期化
          resetFlags();

          if (counter == NUM_TRY) {
            //Processing側に出力する
            outputResult();
            isEnded = true;
          }
        }
      }
      //基準となるtickの更新
      tick = millis();
    }
  }
}

void sendMessage() {
  startTick[counter] = micros();

  //送信処理
  Serial.write(counter);

  isSend = true;
}

void recieveMessage() {
  //読み取り
  int tmp = Serial.read();

  //戻ってきた値がカウンタ値と同じかチェック
  if (counter == tmp) {
    endTick[counter] = micros();
    isRecieve = true;
  }
}

void resetFlags() {
  isSend = false;
  isRecieve = false;

  //カウンタ値インクリメント
  counter++;
}

void calcRoundTripTime(int x) {
  Serial.print(x);
  Serial.print(":RTT=");
  Serial.println(endTick[x] - startTick[x], DEC);

  //平均値計算用
  total += endTick[x] - startTick[x];

  //処理が非同期で終わるから、待つ
  delay(50);
}

void outputResult() {
  for (int i = 0; i < NUM_TRY; i++) {
    calcRoundTripTime(i);
  }

  //平均値出力
  Serial.println(total/NUM_TRY);
}

