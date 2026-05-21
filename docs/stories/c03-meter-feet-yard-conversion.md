# Story 3 Branch Flow: Meter, Feet, Yard Conversion

이 story는 PRD의 기본 단위 변환 계약을 첫 Dual-Track TDD 적용 대상으로 사용한다.

## Contract

- `meter`, `feet`, `yard`는 초기 지원 단위다.
- `1 meter = 3.28084 feet`를 유지한다.
- `1 meter = 1.09361 yard`를 유지한다.
- `feet`와 `yard` 간 변환은 직접 비율이 아니라 meter 허브를 경유한다.
- 변환 결과 비교는 `0.000001` 허용 오차를 사용한다.

## Branch Sequence

1. `red/c03-meter-feet-yard-conversion`
   - UI/Boundary track: 입력 문자열과 출력 원본성 계약을 실패 테스트로 고정한다.
   - Logic/Control-Entity track: meter 허브 비율과 기본 단위 회귀 계약을 실패 테스트로 고정한다.
   - 완료 증거: 실패하는 테스트 이름, 실패 메시지, 연결된 PRD 항목을 PR에 기록한다.

2. `green/c03-meter-feet-yard-conversion`
   - `red/c03-meter-feet-yard-conversion`에서 분기한다.
   - 실패 테스트를 통과시키는 최소 구현만 수행한다.
   - 완료 증거: 관련 Catch2 테스트 통과 결과를 PR에 기록한다.

3. `refactor/c03-meter-feet-yard-conversion`
   - `green/c03-meter-feet-yard-conversion`에서 분기한다.
   - 비율 중복, unit-specific 분기, BCE 책임 위반을 줄인다.
   - 완료 증거: 리팩터링 전후 공개 계약이 같고 회귀 테스트가 통과함을 PR에 기록한다.

## Done Criteria

- RED, GREEN, REFACTOR 작업이 서로 다른 브랜치와 PR로 분리되어 있다.
- `main`에는 GREEN 또는 REFACTOR 완료 산출물만 병합한다.
- `README.md`와 `docs/PRD.md`의 기본 비율 계약이 테스트로 보호된다.
- 리팩터링은 모든 관련 테스트가 GREEN인 상태에서만 수행된다.
