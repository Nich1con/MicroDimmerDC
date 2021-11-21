/* Настройки */
#define START_PWM     200   // Значение ШИМ, до которого идет плавное включение
#define EN_DIS_DELAY  2     // Задержка анимации включения / выключения 
#define FADE_DELAY    10    // Задержка при увеличении / уменьшении яркости
#define LONG_PRESS    500   // Время длительного удержания мс
#define PWM_PIN       0     // Вывод ШИМ
#define BTN_PIN       2     // Вывод кнопки

uint32_t btnTimer = 0;      // Таймер кнопки
bool btnState = false;      // Состояние кнопки
bool btnFlag = false;       // Флаг кнопки
bool pwm_dir = true;        // Направление ШИМ
uint8_t pwm_duty = 0;       // Заполнение ШИМ

void setup() {
  pinMode(PWM_PIN, OUTPUT);     // ШИМ пин как выход
  TCCR0B = TCCR0B & 0xF8 | 1;   // Увеличиваем частоту до 31 кгц
}

void loop() {
  btnState = digitalRead(2);    // Читаем кнопку

  if (btnState && !btnFlag) {   // Кнопка нажимается
    btnFlag = true;             // Ставим флаг
    btnTimer = millis();        // Обвновляем таймер
  }

  if (btnState && btnFlag and millis() - btnTimer >= LONG_PRESS) {  // Кнопка удерживается уже давно
    pwm_duty = constrain(pwm_duty + (pwm_dir ? 1 : -1), 0, 255);    // Увеличиваем / уменьшаем яркость
    analogWrite(0, getBrightCRT(pwm_duty));                         // Выводим на ШИМ
    delay(FADE_DELAY);                                              // Небольшой delay
  }

  if (!btnState && btnFlag) {                             // Кнопку отпустили
    btnFlag = false;                                      // Флаг сбросили
    if (millis() - btnTimer < LONG_PRESS) {               // Если это было короткое нажатие
      if (pwm_duty) {                                     // Если ШИМ > 0
        do {                                              // Плавно снижаем яркость до нуля
          analogWrite(PWM_PIN, getBrightCRT(--pwm_duty));
          delay(EN_DIS_DELAY);
        } while (pwm_duty != 0);
      } else {                                            // Если ШИМ = 0
        do {                                              // Плавно зажигаем
          analogWrite(PWM_PIN, getBrightCRT(++pwm_duty));
          delay(EN_DIS_DELAY);
        } while (pwm_duty < START_PWM);
      }
    } else {                                              // Нажатие было длинным
      pwm_dir = !pwm_dir;                                 // Меняем направление
    }
  }
}

uint8_t getBrightCRT(uint8_t val) {         // CRT гамма (квадратная)
  return ((uint16_t)val * val + 255) >> 8;  // Вычисляем и возвращаем
}
