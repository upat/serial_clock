#!/usr/bin/env python3
# coding: UTF-8
import serial, time, calendar
from datetime import datetime

# 実行環境から現在時刻を取得(unixtime)
time_data = datetime.utcnow()
time_data = calendar.timegm( time_data.utctimetuple() ) + ( 9 * 3600 ) + 2 # タイムゾーン+delay分の調整

# シリアル通信の開始
pserial = serial.Serial( 'COM5', 115200 )
# delayをかけないとコケる
time.sleep( 2 )

# データ送信
pserial.write( ( 'T' + str( time_data ) ).encode( 'utf-8' ) )

# close
pserial.close()

print( 'T' + str( time_data ) )
