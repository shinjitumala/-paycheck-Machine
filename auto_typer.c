#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>
#include<Windows.h>



#define MAX_SCHEDULE 200
#define MAX_LINE 1000
#define INPUT_DELAY 80

//----------------------------------------------------
//
// メルセンヌ・ツイスタのコード（コピペ）
//
//----------------------------------------------------
/* Period parameters */
#define MT_N 624
#define MT_M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt[MT_N]; /* the array for the state vector  */
static int mti=MT_N+1; /* mti==MT_N+1 means mt[MT_N] is not initialized */

/* initializes mt[MT_N] with a seed */
void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<MT_N; mti++) {
        mt[mti] =
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti);
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void init_by_array(unsigned long init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (MT_N>key_length ? MT_N : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=MT_N) { mt[0] = mt[MT_N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=MT_N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=MT_N) { mt[0] = mt[MT_N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= MT_N) { /* generate N words at one time */
        int kk;

        if (mti == MT_N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<MT_N-MT_M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<MT_N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(MT_M-MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[MT_N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[MT_N-1] = mt[MT_M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long genrand_int31(void)
{
    return (long)(genrand_int32()>>1);
}

/* generates a random number on [0,1]-real-interval */
double genrand_real1(void)
{
    return genrand_int32()*(1.0/4294967295.0);
    /* divided by 2^32-1 */
}

/* generates a random number on [0,1)-real-interval */
double genrand_real2(void)
{
    return genrand_int32()*(1.0/4294967296.0);
    /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double genrand_real3(void)
{
    return (((double)genrand_int32()) + 0.5)*(1.0/4294967296.0);
    /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double genrand_res53(void)
{
    unsigned long a=genrand_int32()>>5, b=genrand_int32()>>6;
    return(a*67108864.0+b)*(1.0/9007199254740992.0);
}
/* These real versions are due to Isaku Wada, 2002/01/09 added */
//----------------------------------------------------

//キーボードに一文字入力
void SendKey(unsigned char code){
	keybd_event(code, 0x00, KEYEVENTF_EXTENDEDKEY|0, 0);
	keybd_event(code, 0x00, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);
}

//キーボードにシフトキーを押し下げながら文字入力
void SendKeyShift(unsigned char code){
  keybd_event(160, 0x00, KEYEVENTF_EXTENDEDKEY|0, 0);
  SendKey(code);
  keybd_event(160, 0x00, KEYEVENTF_EXTENDEDKEY|KEYEVENTF_KEYUP, 0);
}

//文字列をキーボード入力
//aからz、AからZ、0から9、スペース、$には対応済み
void SendStr(char*str){
	while(*str){
		if('a' <= *str && 'z' >= *str){
			SendKey(*str - 32);
		}else if('$' == *str){
      SendKeyShift(52);
    }else if('-' == *str){
      SendKey(109);
    }else{
			SendKey(*str);
		}
		str++;
    Sleep(INPUT_DELAY);
	}

  //エンターキーをキーボード入力
  SendKey(13);
}

int main(int argc, char *argv[]){
  //引数の数を確認
  if(argc != 2){
    printf("Usage: ./a.exe \"schedule file name\"");
    return 1;
  }

  //メルセンヌ・ツイスタ初期化
  int k;
  init_genrand((unsigned) time(NULL));
  for(k=0; k<100; k++){
    genrand_int32();
  }
  //ファイルポインタ
  FILE *fp;

  //ファイル読み込み
  fp = fopen(argv[1], "r");
  if(fp == NULL){
    printf("Error reading file!");
    return 1;
  }

  //スケジュール表読み込み
  //スケジュール表変数用意
  typedef struct schedule{
    double start_time;
    double end_time;
  } schedule;

  schedule schedule_table[7][MAX_SCHEDULE];

  int schedule_count[7];

  //ファイル読み込みの用意
  char *p = malloc(sizeof(char) * MAX_LINE);
  char schedule_start = 0;
  int schedule_day = 7;
  char read_start = 0;
  int count = 0;

  printf("SCHEDULE\n");
  //ファイル読み込み
  while(fgets(p, MAX_LINE, fp) != NULL){
    strtok(p, "\r");

    //スケジュール入力開始判定
    if(schedule_start == 1){
      //スケジュール入力の曜日判定
      if(schedule_day == 7){
        //スケジュール入力曜日を入力
        if(strcmp(p, "MON") == 0){
          schedule_day = 1;
          printf("\tMonday\n");
        }else if(strcmp(p, "TUE") == 0){
          schedule_day = 2;
          printf("\tTuesday\n");
        }else if(strcmp(p, "WED") == 0){
          schedule_day = 3;
          printf("\tWednesday\n");
        }else if(strcmp(p, "THU") == 0){
          schedule_day = 4;
          printf("\tThursday\n");
        }else if(strcmp(p, "FRI") == 0){
          schedule_day = 5;
          printf("\tFriday\n");
        }else if(strcmp(p, "SAT") == 0){
          schedule_day = 6;
          printf("\tSaturday\n");
        }else if(strcmp(p, "SUN") == 0){
          schedule_day = 0;
          printf("\tSunday\n");
        }

      }else{

        if(strcmp(p, "ENDMON") == 0 || strcmp(p, "ENDTUE") == 0 || strcmp(p, "ENDWED") == 0 || strcmp(p, "ENDTHU") == 0 || strcmp(p, "ENDFRI") == 0 || strcmp(p, "ENDSAT") == 0 || strcmp(p, "ENDSUN") == 0){
          schedule_count[schedule_day] = count;
          schedule_day = 7;
          count = 0;
          printf("\n");
        }else{
          if(read_start == 0){
            schedule_table[schedule_day][count].start_time = atof(p);
            read_start = 1;
            printf("\t\tstart: %f\n", schedule_table[schedule_day][count].start_time);
          }else{
            schedule_table[schedule_day][count].end_time = atof(p);
            read_start = 0;
            printf("\t\tend: %f\n\n", schedule_table[schedule_day][count].end_time);
            ++count;
          }
        }
      }

      //スケジュール入力終了
      if(strcmp(p, ":endschedule") == 0) break;
    }else{
      //スケジュール入力開始
      if(strcmp(p, ":schedule") == 0) schedule_start = 1;
    }
  }

  //出力文字列の設定読み込み
  //出力文字列の設定変数
  char data_start = 0;
  char *string;
  double interval;
  double margin;
  int mistype_chance;
  int mistype_count;
  int i;
  char *mt_strings[20];

  printf("\nDATA\n");
  //ファイル読み込み
  while(fgets(p, MAX_LINE, fp) != NULL){
    strtok(p, "\r");
    if(data_start == 1){
      p = strtok(p, " ");
      if(strcmp(p, "string") == 0){
        p = strtok(NULL, " ");
        i = strlen(p);
        string = malloc(sizeof(char) * i + 1);
        strcpy(string, p);
        printf("\tstring: %s\n", string);
      }
      if(strcmp(p, "interval") == 0){
        p = strtok(NULL, " ");
        interval = atof(p);
        printf("\tinterval: %f\n", interval);
      }
      if(strcmp(p, "margin") == 0){
        p = strtok(NULL, " ");
        margin = atof(p);
        printf("\tmargin: %f\n", margin);
      }
      if(strcmp(p, "mistype_chance") == 0){
        p = strtok(NULL, " ");
        mistype_chance = atof(p);
        printf("\tmistype_chance: %d\n", mistype_chance);
      }
      if(strcmp(p, "mistype_count") == 0){
        p = strtok(NULL, " ");
        mistype_count = atof(p);
        printf("\tmistype_count: %d\n", mistype_count);
        //注意
        i = 0;
      }
      if(strcmp(p, "mistype") == 0){
        p = strtok(NULL, " ");
        mt_strings[i] = malloc(sizeof(char) * strlen(p) + 1);
        strcpy(mt_strings[i], p);
        printf("\tmistype%d: %s\n", i, mt_strings[i]);
        i++;
      }
      //データ入力終了
      if(strcmp(p, ":enddata") == 0) break;
    }else{
      //データ入力開始
      if(strcmp(p, ":data") == 0) data_start = 1;
    }
  }

  //ファイル読み込み終了
  fclose(fp);


  //ログ記録
  printf("\nLOGS\n");

  //キーボード自動出力
  //初期設定
  //現在時刻の取得用意
  time_t timer;
  struct tm *local;

  //現在時間
  timer = time(NULL);
  local = localtime(&timer);

  //現在時刻データ
  int Wday = local->tm_wday;
  int schedule_pos;
  double c_time = local->tm_hour + ((double) local->tm_min / 60);
  char waiting;
  char end_day = 0;
  double w_time = 0;

  //はじめ、どこの時間にいるかを探す
  schedule_pos = 0;
  while(schedule_count[Wday] > schedule_pos){
    if(c_time >= schedule_table[Wday][schedule_pos].start_time && c_time <= schedule_table[Wday][schedule_pos].end_time){
      waiting = 0;
      printf("\ttime: %f -> Currently start time!\n", c_time);
      break;
    }else if(c_time >= schedule_table[Wday][schedule_pos].end_time){
      waiting = 1;
    }else if(c_time <= schedule_table[Wday][schedule_pos].start_time && waiting == 1){
      printf("\ttime: %f -> waiting for time: %f\n", c_time, schedule_table[Wday][schedule_pos].start_time);
    }else{
      printf("\ttime: %f -> No schedule for today!\n", c_time);
      end_day = 1;
    }
    schedule_pos++;
  }

  //始める前に５秒待つ
  Sleep(5000);

  //ループ
  while(1){
    //現在時刻を取得
    timer = time(NULL);
    local = localtime(&timer);
    c_time = local->tm_hour + ((double) local->tm_min / 60);

    if(end_day == 1){
      //一日のスケジュールがすでに終了している場合
      if(Wday == 6){
        Wday = 0;
      }else{
        Wday++;
      }

      if(Wday == local->tm_wday){
        schedule_pos = 0;
        end_day = 0;
      }
    }else{
      //一日のスケジュールがまだ終わっていない場合
      //一日のスケジュールを確認
      if(c_time >= schedule_table[Wday][schedule_pos].start_time && c_time <= schedule_table[Wday][schedule_pos].end_time){
        waiting = 0;
      }else if(c_time >= schedule_table[Wday][schedule_pos].end_time && waiting == 0){
        waiting = 1;
        schedule_pos++;
      }else if(c_time <= schedule_table[Wday][schedule_pos].start_time && waiting == 1){

      }else{
        end_day = 1;
      }

      if(waiting == 0){
        if(w_time <= 0){
          if((int) 100 * genrand_real1() <= mistype_chance){
            i = (int) mistype_count * genrand_real1();
            SendStr(mt_strings[i]);
            printf("\ttime: %f -> mistyped: %s\n", c_time, mt_strings[i]);
            Sleep((int) 1000 + 1000 * genrand_real1());
          }
          SendStr(string);
          w_time = interval + margin * genrand_real1();
          printf("\ttime: %f -> typed: %s\n", c_time, string);
          printf("\ttime: %f -> wait: %f\n", c_time, w_time);
        }else{
          w_time -= 0.000278;
        }
      }
    }

    //1秒休む
    Sleep(1000);
  }

  //メモリ開放
  free(string);

  return 0;
}
