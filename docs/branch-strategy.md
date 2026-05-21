# Git Branch Strategy

이 저장소는 Dual-Track UI + Logic TDD를 `RED -> GREEN -> REFACTOR` 순서로 진행한다. `main`은 항상 통과 가능한 안정 기준선으로 유지하고, 실패 테스트가 포함될 수 있는 RED 작업은 별도 브랜치에 격리한다.

## Branch Roles

- `main`: GREEN 또는 REFACTOR 완료 산출물만 병합하는 안정 브랜치.
- `spec`: PRD, 요구사항, Gherkin, 작업 계약을 관리하는 문서 기준선.
- `RED`: 실패 테스트를 모아 검토할 수 있는 장기 통합 브랜치. `main`으로 직접 병합하지 않는다.
- `red/<story-id>-<slug>`: UI/Boundary 테스트와 Logic/Control-Entity 테스트를 먼저 작성하고 실패를 확인하는 짧은 story 브랜치.
- `green/<story-id>-<slug>`: 대응 RED 브랜치에서 분기해 테스트를 통과시키는 최소 구현 브랜치.
- `refactor/<story-id>-<slug>`: 대응 GREEN 브랜치에서 분기해 동작 변경 없이 구조만 개선하는 브랜치.
- `release/<version-or-phase>`: 여러 story가 사용 가능한 단위로 모였을 때만 선택적으로 만든다.

## Phase Gates

### RED

- 기준 브랜치: `spec`, 최신 `main`, 또는 기존 RED 통합 브랜치.
- 작업 범위: 실패하는 Catch2 테스트, 테스트 보고서, 계약 문서.
- 완료 조건: 실패 원인이 구현 부재 또는 계약 미충족임을 설명할 수 있어야 한다.
- 병합 정책: `main` 병합 금지. 필요하면 `RED` 통합 브랜치에만 병합한다.

### GREEN

- 기준 브랜치: 대응하는 `red/<story-id>-<slug>`.
- 작업 범위: 현재 실패 테스트를 통과시키는 최소 구현.
- 완료 조건: 관련 테스트가 통과하고 Boundary-Control-Entity 의존성 방향을 유지한다.
- 병합 정책: 리팩터링이 필요 없으면 `main`으로 PR을 열 수 있고, 구조 개선이 필요하면 `refactor/<story-id>-<slug>`로 넘긴다.

### REFACTOR

- 기준 브랜치: 대응하는 `green/<story-id>-<slug>`.
- 작업 범위: 동작 변경 없는 구조 개선, 중복 제거, BCE 책임 재배치.
- 완료 조건: 관련 테스트와 회귀 테스트가 계속 통과한다.
- 병합 정책: 최종 PR로 `main`에 병합한다.

## PR Checks

모든 PR은 다음 정보를 남긴다.

- 현재 단계: `RED`, `GREEN`, `REFACTOR` 중 하나.
- 연결된 PRD/Story: 예 `PRD Story 3`.
- 변경 범위: UI/Boundary track, Logic/Control-Entity track, 또는 둘 다.
- 실행 명령과 결과: 예 `ctest --test-dir build`.
- 단계별 증거: RED 실패 로그, GREEN 통과 로그, REFACTOR 회귀 통과 로그 중 해당 항목.

단계별 필수 조건은 다음과 같다.

- RED PR: 실패가 기대된 테스트이며, 실패 이유가 계약과 연결되어야 한다.
- GREEN PR: 관련 테스트가 통과해야 하며, 리팩터링을 섞지 않는다.
- REFACTOR PR: 동작 변경이 없어야 하며, 공개 계약과 오류 패턴을 변경하지 않는다.

## CI Pass Conditions

현재 저장소에는 아직 CMake/Catch2 실행 구성이 없으므로, 자동 CI가 추가되기 전까지 PR 본문에 수동 실행 결과를 남긴다. CMake/Catch2 구성이 추가되면 `green/*`, `refactor/*`, `main` 대상 PR은 다음 명령을 필수 통과 조건으로 둔다.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

`red/*` PR은 실패 테스트를 포함할 수 있으므로 전체 CI 실패가 정상일 수 있다. 이 경우 실패 테스트 이름, 실패 메시지, 연결된 계약을 PR 본문에 기록하고 `main` 병합 대상에서 제외한다.

## First Story Application

첫 적용 story는 PRD의 기본 단위 변환 계약인 `Story 3`으로 시작한다. 세부 흐름은 [stories/c03-meter-feet-yard-conversion.md](./stories/c03-meter-feet-yard-conversion.md)에 고정한다.

- RED 브랜치: `red/c03-meter-feet-yard-conversion`
- GREEN 브랜치: `green/c03-meter-feet-yard-conversion`
- REFACTOR 브랜치: `refactor/c03-meter-feet-yard-conversion`

RED 단계에서는 `meter`, `feet`, `yard`의 README 기준 비율과 feet-yard 간 meter 허브 경유를 테스트로 고정한다. GREEN 단계에서는 해당 테스트만 통과시키는 최소 구현을 수행한다. REFACTOR 단계에서는 비율 중복과 unit-specific 분기를 줄이되, 출력 계약과 오류 계약은 변경하지 않는다.

## Commit Message Examples

```text
test: add RED coverage for meter hub conversion
feat: pass length conversion through meter ratio registry
refactor: isolate conversion ratios in unit definitions
```
