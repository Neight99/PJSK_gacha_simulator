#include <algorithm>
#include <iostream>
#include <random>
#include <set>
#include <thread>
#include <vector>

using namespace std;

typedef long long ll;

// 单张UP卡概率（千分数）
const int UP_PROBABILITY = 4;
// 四星卡概率（千分数）
const int FOUR_PROBABILITY = 30;
// UP卡数量
const int UP_NUM = 3;
// 天井抽数
const int GUARANT_NUM = 300;
// 是否是限定
const bool IS_LIMITED = false;
// 多线程数
const int THREAD_NUM = 10;
// 模拟总次数
const int simulateNum = 1e4;

int sumGachaTimes = 0, sumFourTimes = 0, sumUpTimes = 0;
int getAllInOneGuarant = 0, getAllInTwoGuarant = 0;

void simulate(int simulateTimes) {
    vector<int> normalPool, normalGuarantPool, specialGuarantPool;
    set<int> upSet;

    random_device randomDevice;
    mt19937 generator(randomDevice());

    // Init pool.
    for (auto i = 1; i <= UP_NUM; i++) {
        for (auto j = 0; j < UP_PROBABILITY; j++) {
            normalPool.push_back(i);
            normalGuarantPool.push_back(i);
        }
        upSet.insert(i);
        specialGuarantPool.push_back(i);
    }
    for (auto i = 0; i < FOUR_PROBABILITY - UP_PROBABILITY * UP_NUM; i++) {
        normalPool.push_back(0);
        normalGuarantPool.push_back(0);
    }
    for (auto i = 0; i < 1000 - FOUR_PROBABILITY; i++) {
        normalPool.push_back(-1);
    }
    shuffle(normalPool.begin(), normalPool.end(), generator);
    shuffle(normalGuarantPool.begin(), normalGuarantPool.end(), generator);
    shuffle(specialGuarantPool.begin(), specialGuarantPool.end(), generator);

    // Simulate.
    for (auto i = 0; i < simulateTimes; i++) {
        set<int> notGotSet(upSet);
        int gachaTimes = 0;
        bool canGuarant = false;
        while (notGotSet.size() > 0) {
            if (canGuarant) {
                canGuarant = false;
                notGotSet.erase(notGotSet.begin());
                continue;
            }

            int item = -10;
            gachaTimes++;
            sumGachaTimes++;
            if (!IS_LIMITED) {
                if (gachaTimes % 100 == 0) {
                    shuffle(specialGuarantPool.begin(),
                            specialGuarantPool.end(), generator);
                    item = specialGuarantPool.back();
                } else if (gachaTimes % 50 == 0) {
                    shuffle(normalGuarantPool.begin(), normalGuarantPool.end(),
                            generator);
                    item = normalGuarantPool.back();
                }
            }
            if (item == -10) {
                shuffle(normalPool.begin(), normalPool.end(), generator);
                item = normalPool.back();
            }

            if (item >= 0) {
                sumFourTimes++;
                if (item > 0) {
                    sumUpTimes++;
                    auto iter = notGotSet.find(item);
                    if (iter != notGotSet.end()) {
                        notGotSet.erase(iter);
                    }
                }
            }

            if (gachaTimes % GUARANT_NUM == 0) {
                canGuarant = true;
            }
        }

        if (gachaTimes <= GUARANT_NUM) {
            getAllInOneGuarant++;
        } else if (gachaTimes <= 2 * GUARANT_NUM) {
            getAllInTwoGuarant++;
        }
    }
}

int main() {
    vector<thread> simulateThreads;
    for (auto i = 0; i < THREAD_NUM; i++) {
        simulateThreads.push_back(thread(simulate, simulateNum / THREAD_NUM));
    }
    for (auto iter = simulateThreads.begin(); iter != simulateThreads.end();
         iter++) {
        iter->join();
    }

    cout << "Get all up card in one guarant: " << getAllInOneGuarant
         << " Prob :" << 1.0 * getAllInOneGuarant / simulateNum << endl;
    cout << "Get all up card in two guarant: " << getAllInTwoGuarant
         << " Prob :" << 1.0 * getAllInTwoGuarant / simulateNum << endl;
    cout << "4x Prob: " << 1.0 * sumFourTimes / sumGachaTimes << endl;
    cout << "up Prob: " << 1.0 * sumUpTimes / sumGachaTimes << endl;

    return 0;
}