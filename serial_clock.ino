#include <Adafruit_GFX.h>
#include "Fonts/FreeSans12pt7b.h"
#include <Adafruit_TFTLCD.h>
#include <TimeLib.h>

#define TIME_HEADER  "T"        /* 時間 */
#define DEFAULT_TIME 1500000000 /* 日時がデフォルト値かの判定用 */

#define LCD_CS    A3
#define LCD_CD    A2
#define LCD_WR    A1
#define LCD_RD    A0
#define LCD_RESET A4

#define BLACK 0x0000
#define GREEN 0x07E0

time_t now_data;  /* 最新日時 */
time_t disp_data; /* 最終画面描画の日時 */

int day_pre;
int hour_pre;

/* 曜日変換用の文字列 */
const char week_day[7][8] = { "(SUN)", "(MON)", "(TUE)", "(WED)", "(THU)", "(FRI)", "(SAT)" };

Adafruit_TFTLCD tft( LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET );

void setup() {
  Serial.begin( 115200 );
  tft.begin( 0x9341 ); /* ILI9341 */

  disp_data = now();

  /* 画面表示の向き */
  tft.setRotation( 1 );
  /* フォント設定 */
  tft.setFont( &FreeSans12pt7b );
  tft.setTextColor( GREEN );
  /* 初回表示 */
  set_display();
}

void loop() {
  /* データを受信した場合 */
  if( Serial.available() )
  {
    read_serial();
  }
  
  /* 時刻が更新された時 */
  if( now_data != now() )
  {
    now_data = now();

    /* 毎分 or デフォルト値だった場合更新 */
    if( 0 == second( now_data ) || DEFAULT_TIME > disp_data )
    {
      disp_data = now_data;
      set_display();
    }
  }

  delay( 100 - millis() % 100 );
}

/* USB経由でシリアル通信でデータ取得 */
void read_serial()
{
  unsigned long pctime;

  if( Serial.find( TIME_HEADER ) )
  {
    pctime = Serial.parseInt();

    /* ヘッダー判定 */
    if( pctime >= DEFAULT_TIME )
    {
      /* 時刻補正 */
      setTime( pctime );
    }
  }
}

/* TFTシールドへ描画 */
void set_display()
{
  char day_data[12];
  char time_data[12];

  /* 日付・時間のフォーマット形式 */
  const char* format_day = "%04d.%02d.%02d";
  const char* format_time = "%02d:%02d";

  sprintf( day_data, format_day, year( now_data ), month( now_data ), day( now_data ) );
  sprintf( time_data, format_time, hour( now_data ), minute( now_data ) );

  if( day_pre != day( now_data ) )
  {
    /* 日付が変わった場合、全て再描画 */
    tft.fillScreen( BLACK );
    
    /* 曜日のデータをセット */
    tft.setTextSize( 1 );
    tft.setCursor( 245, 60 );
    tft.println( week_day[weekday( now_data ) - 1] );

    /* 日付のデータをセット */
    tft.setTextSize( 2 );
    tft.setCursor( 10, 60 );
    tft.println( day_data );

    /* 時刻用文字サイズ指定 */
    tft.setTextSize( 5 );
  }
  else if( hour_pre != hour( now_data ) )
  {
    /* 時間が変わった場合、時刻部分のみ再描画 */
    tft.fillRect( 0, 100, 320, 200, BLACK );
  }
  else
  {
    /* 分単位で変わった場合、分秒のみ再描画 */
    tft.fillRect( 170, 100, 150, 200, BLACK );
  }

  /* 時刻のデータをセット */
  tft.setCursor( 10, 200 );
  tft.println( time_data );

  /* 前回値更新 */
  day_pre  = day( now_data );
  hour_pre = hour( now_data );
}
