만약 실습 시 데이터를 UART로 보내고 싶으면

해커보드 왼쪽 하단에 UART CON(connect)/N.C(not connect) 가 있습니다. 이를 CON으로 옮긴 후 

해커보드 어태커, 디펜더 하단 D0(RX), D1(TX) 핀을 사용하면 됩니다.
 
어태커의 D0 핀을 디펜더의 D1 핀과 연결하고
	D1 핀을 디펜더의 D0 핀과 연결하면 됩니다.

코드 작성 시 해커보드의 UART RX, TX 핀은 현재 사용하는 해커보드 기준 RX : 0, TX : 1 입니다.

	#include <SoftwareSerial.h>

	SoftwareSerial uart(0, 1);

양쪽에 이렇게 설정해야 합니다.

근데 해커보드에 코드를 업로드하는 핀도 D0, D1으로 공유하기 때문에

코드를 업로드 하기 전에 무조건 UART연결을 해제하고 코드를 업로드 해야합니다.(안 그러면 에러남)
업로드를 완료하면 다시 선을 연결 후 진행하면 됩니다.

--------------------------------------------------------------------

현재 실습에 사용한 PM-PRO는 virus total 업로드 시 악성코드가 감지되기 때문에 가상머신에서 실습을 하시면 좋고, 장비 지원이 된다면 ACR-122u 나 Proxmark3를 구매해서 사용하는게 더 다양한 해킹 모듈과 라이브러리를 제공하고, 칼리리눅스에서도 추가 실습을 진행 할 수 있어 좋습니다.

--------------------------------------------------------------------

첨부한 코드 중 attacker_board와 defender_board는 한 쌍으로 각각 attacker와 defender에 업로드 후 사용하시고,
find_key_for_block은 단독으로 태그한 RFID에 특정 블록에(수정가능, 현재는 29블럭(8섹터) ) 키값을 대입하는 코드입니다.
default_key는 이미 정해진 키값(변경가능)을 특정 블럭에 시도해 봅니다.

access_reader는 uid가 저장된 값과 같으면 lcd에 성공/실패여부가 보입니다. 복제후 정상적으로 인식이 되는지 확인용입니다.

