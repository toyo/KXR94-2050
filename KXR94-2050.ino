#include <Arduino.h>

int sort_unsigned_desc(const void *cmp1, const void *cmp2)
{ 
  float a = *((unsigned *)cmp1);
  float b = *((unsigned *)cmp2);

  return a > b ? -1 : (a < b ? 1 : 0);
  //return b - a;
}

bool run; 

void setup()
{
  Serial.begin(115200);
  run = false;
} 

void loop()
{
  int x = analogRead(3);
  int y = analogRead(4);
  int z = analogRead(5);

  const int count = 100;           // 100 data
  const int continuous = 10;       // 5th (0.5 sec. in 5sec )
  const unsigned threshold = 1500; //milligal, gal = cm/s^2

  static unsigned long sumx;
  static unsigned long sumy;
  static unsigned long sumz;
  static int n;
  static float expsmoothx;
  static float expsmoothy;
  static float expsmoothz;
  static unsigned sums[count];

  if (n == 0)
  {
    expsmoothx = float(x);
    expsmoothy = float(y);
    expsmoothz = float(z);
  }
  else
  {
    expsmoothx = expsmoothx * 0.94 + float(x) * 0.06;
    expsmoothy = expsmoothy * 0.94 + float(y) * 0.06;
    expsmoothz = expsmoothz * 0.94 + float(z) * 0.06;
  }

  sumx += x;
  sumy += y;
  sumz += z;
  n++;

  float xx = expsmoothx - float(sumx) / n;
  float yy = expsmoothy - float(sumy) / n;
  float zz = expsmoothz - float(sumz) / n;

  if (n == count)
  {
    sumx = x;
    sumy = y;
    sumz = z;
    n = 1;
    if (!run)
    {
      run = true;
      Serial.println("run!");
    }
  }

  sums[n - 1] = sqrt(xx * xx + yy * yy + zz * zz) * 32; // 0 - 32768
  if (run)
  {
    unsigned sumsort[count];
    memcpy(sumsort, sums, sizeof(sumsort));
    qsort(sumsort, count, sizeof(sumsort[0]), sort_unsigned_desc);
    int milligal = float(sumsort[continuous]) / 32768 * 5 * 980 * 1000;
    if (milligal > threshold)
    {
      double shindo = double(2) * log(float(milligal) / 1000) / log(10) + 0.94;
      Serial.print("震度");
      Serial.print(shindo);
      Serial.print(" milliGal:");
      Serial.print(milligal);
      Serial.println();
    }
  }

  delay(5);
}
