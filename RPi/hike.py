MET_HIKING = 6
TIME_PER_STEP = 0.5
HIKER_WEIGHT = 75.5
BAG_WEIGHT = 5

class HikeSession:
    id = 0
    cm = 0
    steps = 0
    kcal = -1
    coords = []

    # represents a computationally intensive calculation done by lazy execution.
    def calc_kcal(self):
        self.kcal = MET_HIKING * (HIKER_WEIGHT + BAG_WEIGHT) * TIME_PER_STEP * self.steps / 3600

    def __repr__(self):
        return f"HikeSession{{{self.id}, {self.cm}(cm), {self.steps}(steps), {self.kcal:.2f}(kcal)}}"

def to_list(s: HikeSession) -> list:
    return [s.id, s.cm, s.steps, s.kcal]

def from_list(l: list) -> HikeSession:
    s = HikeSession()
    s.id = l[0]
    s.cm = l[1]
    s.steps = l[2]
    s.kcal = l[3]
    return s