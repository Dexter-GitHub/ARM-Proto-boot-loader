# ARM-Proto-boot-loader
T5-Auto-RMeasure-V1-Prototype STM32 부트로더
<br>
<br>

### 테스트 목적
* STM32 부트로더 에서 Ethernet(W5500)을 이용한 Flash Download
<br>
  
## Project 정보
* STM32F103VGT6(LQFP100) 
* STM32CubeMX Version 6.1.2 사용 (핀맵 설정 및 베이스코드 생성툴 - 무료 라이센스)
* STM32CubeIDE Version 1.10.0 사용 (GNU Tools for STM32 gmake 컴파일러 - 무료 라이센스)
<br>

## Issues Report
* USART 활성화 후 <code>printf</code> 사용시 <code>\r\n</code> 개행문자를 사용하지 않아
  출력이 안되어 삽질 ...(동작 안하는 것으로 오인)
