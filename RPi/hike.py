MET_HIKING = 6
KM_PER_STEP = 0.75/1000 #km/1
HIKER_WEIGHT = 75.5 #kg
HIKING_SPEED = 4.5 #km/s
BAG_WEIGHT = 10 #kg
KCAL_PER_STEP = MET_HIKING * KM_PER_STEP/HIKING_SPEED * (HIKER_WEIGHT + BAG_WEIGHT) # = 0.0855

class HikeSession:
    id = 0
    cm = 0
    steps = 0
    kcal = -1
    coords = []

    # represents a computationally intensive calculation done by lazy execution.
    def calc_kcal(self):
        self.kcal = self.step * KCAL_PER_STEP

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