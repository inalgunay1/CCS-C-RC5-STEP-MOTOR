
#define LCD_RS_PIN PIN_D0
#define LCD_RW_PIN PIN_D1
#define LCD_ENABLE_PIN PIN_D2
#define LCD_DATA4 PIN_D3
#define LCD_DATA5 PIN_D4
#define LCD_DATA6 PIN_D5
#define LCD_DATA7 PIN_D6
#define short_time      700                      
#define   med_time     1200                      
#define  long_time     2000 
#include <16f877.h>
#fuses XT,NOWDT,NOPROTECT,NOBROWNOUT,NOLVP,NOPUT,NOWRT,NODEBUG,NOCPD
#use delay(clock = 8MHz)
#include <lcd.c>
#use fast_io(B)
int i ; 
short rc5_ok = 0, toggle_bit;
unsigned int8 rc5_state = 0, j, address, command;
unsigned int16 rc5_code;
#INT_EXT                                         
void ext_isr(void){
  unsigned int16 time;
  if(rc5_state != 0){
    time = get_timer1();                         
    set_timer1(0);                               
  }
  switch(rc5_state){
   case 0 :                                      
    setup_timer_1( T1_INTERNAL | T1_DIV_BY_2 );  
    set_timer1(0);                               
    rc5_state = 1;                               
    j = 0;
    ext_int_edge( L_TO_H );                      
    return;
   case 1 :                                        
    if((time > long_time) || (time < short_time)){         
      rc5_state = 0;                             
      setup_timer_1(T1_DISABLED);
      ext_int_edge( H_TO_L );                    
      return;
    }
    bit_set(rc5_code, 13 - j);
    j++;
    if(j > 13){                                  
      rc5_ok = 1;                                
      disable_interrupts(INT_EXT);               
      return;
    }
      if(time > med_time){                       
        rc5_state = 2;                           
        if(j == 13){                             
          rc5_ok = 1;                            
          bit_clear(rc5_code, 0);                
          disable_interrupts(INT_EXT);           
          return;
        }
      }
      else                                       
        rc5_state = 3;                           
      ext_int_edge( H_TO_L );                    
      return;
   case 2 :                                        
    if((time > long_time) || (time < short_time)){
      rc5_state = 0;                             
      setup_timer_1( T1_DISABLED);               
      return;
    }
    bit_clear(rc5_code, 13 - j);
    j++;
    if(time > med_time)                         
      rc5_state = 1;                             
    else                                         
      rc5_state = 4;                             
    ext_int_edge( L_TO_H );                      
    return;
   case 3 :                                        
    if((time > med_time) || (time < short_time)){           
      setup_timer_1(T1_DISABLED);                
      rc5_state = 0;
      return;
    }
    else                                         
      rc5_state = 1;                             
    ext_int_edge( L_TO_H );
    return;
   case 4 :                                        
    if((time > med_time) || (time < short_time)){           
      setup_timer_1(T1_DISABLED);                
      rc5_state = 0;                             
      ext_int_edge( H_TO_L );                    
      return;
    }
    else                                         
      rc5_state = 2;                             
    ext_int_edge( H_TO_L );                      
    if(j == 13){                                 
      rc5_ok = 1;                                
      bit_clear(rc5_code, 0);                    
      disable_interrupts(INT_EXT);               
    }
  }
}
#INT_TIMER1                                      
void timer1_isr(void){
  rc5_state = 0;                                 
  ext_int_edge( H_TO_L );                        
  setup_timer_1(T1_DISABLED);                    
  clear_interrupt(INT_TIMER1);                   
}

void main()
{
               
                      
  lcd_init();                                   
  lcd_putc('\f');                                
  enable_interrupts(GLOBAL);                     
  enable_interrupts(INT_EXT_H2L);               
  clear_interrupt(INT_TIMER1);                   
  enable_interrupts(INT_TIMER1);                 
  lcd_gotoxy(1, 1);
  lcd_putc("ADS:0x00  TGL: 0");
  lcd_gotoxy(1, 2);
  lcd_putc("CMD:0x00");                       
   while(TRUE)
   {
   if(rc5_ok){                                  
      rc5_ok = 0;                                
      rc5_state = 0;
      setup_timer_1(T1_DISABLED);                
      toggle_bit = bit_test(rc5_code, 11);       
      address = (rc5_code >> 6) & 0x1F;          
      command = rc5_code & 0x3F;                 
      lcd_gotoxy(16, 1);
      printf(lcd_putc,"%1u",toggle_bit);         
      lcd_gotoxy(7, 1);
      printf(lcd_putc,"%2LX",address);           
      lcd_gotoxy(7, 2);
      printf(lcd_putc,"%2LX",command);           
      enable_interrupts(INT_EXT_H2L);            
      
      
      if(command== 0x07 ) { 
        for ( i=0; i<=50 ; i++) {
      output_d(0b00000110);
      delay_ms(10);
      output_d(0b00000101);
      delay_ms(10); 
      output_d(0b00001001);
      delay_ms(10); 
      output_d(0b00001010); 
      delay_ms(10);
        } 
      } 
      
     
        if(command == 0x06) { 
         for ( i=0; i<= 50 ; i++ ) {
         output_d(0b00000101);
         delay_ms(10);
         output_d(0b00000110);
         delay_ms(10);
         output_d(0b00001010);
         delay_ms(10);
         output_d(0b00001001);
         delay_ms(10);
         }
        }   
   }
}
}
