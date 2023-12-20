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
<br>

## 23-12-20 테스트 내용
* W5500 Ftp Client를 사용하여 DeskTop PC Ftp Server에 접근하여 Application.bin 파일을 다운로드 한다.
* 다운로드한 파일은 외장 Flash 메모리의 파일시스템에 저장한다.
* 파일시스템을 이용하여 파일을 open/read 하여 내장 Flash 메모리에 저장한다.
<br>

<p>
  부트롬 크기는 약 209.05KB로 Page110(0x8037000)까지 사용하고, 그외 영역은 Application영역으로 한다.<br>
  Application은 시작 주소는 0x8037000 사이즈는 0xC9000으로 빌드하여 bin파일을 생성하여 사용하였다.
</p>
