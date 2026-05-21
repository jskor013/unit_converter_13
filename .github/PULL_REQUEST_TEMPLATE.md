## Stage

- [ ] RED: failing UI/Boundary and Logic/Control-Entity tests define the contract.
- [ ] GREEN: the current failing tests pass with the smallest behavior change.
- [ ] REFACTOR: behavior is unchanged and relevant tests remain green.

## Linked Contract

- PRD/Story:
- Branch:
- Track: UI/Boundary, Logic/Control-Entity, or both.

## Evidence

- RED evidence:
- GREEN evidence:
- Refactor scope:
- Test command/result:

## Checklist

- [ ] Branch name follows `red/`, `green/`, or `refactor/` story naming.
- [ ] This PR does not mix RED, GREEN, and REFACTOR work.
- [ ] Boundary, Control, and Entity responsibilities remain separated.
- [ ] Public contracts, error codes, and message patterns are preserved unless RED tests were updated first.
- [ ] README meter, feet, and yard ratio regression coverage is preserved.
