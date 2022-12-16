#include "brain.h"

using namespace std;

/**
 * For this LAB, you only need to change this file.
 * You should write 4 functions defined in MyBrain class
 * Notice that MyBrain has no direct access to the Game.
 * BTW, the brain here (without changes) is just the "Straightforward AI" I mentioned in the lab description,
 * you can submit the this file without changes to JOJ can see what score it can get.
 */

// TODO: Create your data structure / class / functions here (optional)

class MyBrain : public Brain
{
    int hand[30]{};
    int knowntile[30]{};
    int nPair = 0, nMeld = 0, nPartner = 0;
    int temp_possible = 0;
    int *pdistance = nullptr;
    int *p_possible = nullptr;
    Tile *p_tileToBeDiscarded = nullptr;
    int distance = 10, possible = 0;
    Tile tileToBeDiscarded{0};
    int ifalone[30]{};

    MyBrain *removePair(int i)
    {
        this->hand[i] -= 2;
        this->nPair++;
        return this;
    }

    MyBrain *removeTriplet(int i)
    {
        this->hand[i] -= 3;
        this->nMeld++;
        return this;
    }

    MyBrain *removeSequence(int i)
    {
        this->hand[i]--;
        this->hand[i + 1]--;
        this->hand[i + 2]--;
        this->nMeld++;
        return this;
    }

    MyBrain *removePartner(int i, int j)
    {
        this->hand[i]--;
        this->hand[j]--;
        this->nPartner++;
        if (j == i)
            this->temp_possible += 4 - this->knowntile[i];
        if (j == i + 1)
            this->temp_possible += 8 - this->knowntile[i + 2] - this->knowntile[i - 1];
        if (j == i + 2)
            this->temp_possible += 4 - this->knowntile[i + 1];
        return this;
    }

    void dfs_checkOut()
    {
        int temp_distance = 8 - 2 * this->nMeld - this->nPartner - this->nPair;
        if (temp_distance < *this->pdistance)
        {
            *this->pdistance = temp_distance;
            *this->p_possible = this->temp_possible;
            for (int i = 0; i < 30; i++)
            {
                if (hand[i] > 0)
                {
                    *this->p_tileToBeDiscarded = Tile(i);
                    break;
                }
            }
        } 
        else if (temp_distance == *this->pdistance)
        {
            if (*this->p_possible < this->temp_possible)
            {
                *this->p_possible = this->temp_possible;
                for (int i = 0; i < 30; i++)
                {
                    if (this->hand[i] > 0)
                    {
                        *this->p_tileToBeDiscarded = Tile(i);
                        break;
                    }
                }
            }
        }
    }

    void dfs(int i)
    {
        if (i == 30)
        {
            MyBrain(*this).dfs_checkOut();
        } else if (this->hand[i] == 0)
            MyBrain(*this).dfs(i + 1);
        else
        {
            if (this->nPair == 0 && this->hand[i] >= 2)
                MyBrain(*this).removePair(i)->dfs(i);
            if (this->hand[i] >= 3 && nMeld+nPartner<4)
                MyBrain(*this).removeTriplet(i)->dfs(i);
            if (i % 10 <= 7 && this->hand[i] >= 1 && this->hand[i + 1] >= 1 && this->hand[i + 2] >= 1 && nMeld+nPartner<4)
                MyBrain(*this).removeSequence(i)->dfs(i);
            if (this->hand[i] > 0 && this->nMeld + this->nPartner < 4)
            {
                if (i % 10 <= 8 && hand[i + 1] > 0)
                    MyBrain(*this).removePartner(i, i + 1)->dfs(i);
                if (i % 10 <= 7 && hand[i + 2] > 0)
                    MyBrain(*this).removePartner(i, i + 2)->dfs(i);
                if (hand[i] >= 2)
                    MyBrain(*this).removePartner(i, i)->dfs(i);
            }
            MyBrain(*this).dfs(i + 1);
        }
    }
    // TODO: Create your class member here.

    /**
     * @brief this method will be called in Game when you draw a tile from the wall,
     * at the beginning of the game, you will draw 13 tiles as your initial hand
     * @param tileDrawn the tile you draw
     */
    void getDraw(Tile tileDrawn) override
    {
        this->hand[tileDrawn.toInt()]++;
        this->knowntile[tileDrawn.toInt()]++;
        // TODO: Complete this function

    }

    /**
     * @brief this method will be called in Game when you should discard a tile
     * @return the tile you want to discard from your hand
     */
    Tile requestDiscard() override
    {
        // TODO: Complete this function
        this->knowntile[0] = 4;
        this->knowntile[10] = 4;
        this->knowntile[20] = 4;
        int pairsnum = 0;
        //for()
        for (int i = 1; i < 30; i++)
        {
            if (hand[i] == 2)
                pairsnum++;
            else if (hand[i] == 4)
                pairsnum += 2;
        } // pairsnum
        int num = 0;
        int alone = -1;
        int suitsnum[3]{};
        for (int suit = 0; suit < 3; ++suit)
        {
            bool ifSuitExist = false;
            for (int ordinal = 1; ordinal <= 9; ++ordinal)
            {
                if (this->hand[10 * suit + ordinal] > 0)
                {
                    ifSuitExist = true;
                    suitsnum[suit] += this->hand[10 * suit + ordinal];
                }

            }
            if (ifSuitExist)
                num++;
            if (suitsnum[suit] == 1)
                alone = suit;
        } //suitsnum and num of suits
        if (num == 3)  // 3 kinds
        {
            int min = 0;
            for (int i = 0; i < 3; i++)
                if (suitsnum[i] < suitsnum[min])
                    min = i;
            for (int i = min * 10 + 1; i < min * 10 + 10; i++)
                if (this->hand[i] > 0)
                {
                    this->tileToBeDiscarded = Tile(i);
                    break;
                }
        } else if (alone != -1)
        {
            for (int i = alone * 10 + 1; i < alone * 10 + 10; i++)
                if (this->hand[i] > 0)
                {
                    this->tileToBeDiscarded = Tile(i);
                    break;
                }
        } else
        {
            int distance_pairs = 6 - pairsnum;
            if (distance_pairs <= 0)
            {
                int min_unpairpos;
                for (int i = 1; i < 30; i++)
                {
                    if (this->hand[i] == 1 || this->hand[i] == 3)
                    {
                        min_unpairpos = i;
                        this->tileToBeDiscarded = Tile(min_unpairpos);
                    }
                }
            } else
            {
                //nPair=0;
                this->temp_possible = 0;
                this->distance = 8;
                this->possible = 0;
                pdistance = &this->distance;
                p_possible = &this->possible;
                p_tileToBeDiscarded = &tileToBeDiscarded;
                dfs(1);
            }
        }
        this->hand[tileToBeDiscarded.toInt()]--;
        return tileToBeDiscarded;
    }

    /**
     * @brief this method will be called in Game when the other player discards a tile
     * @param tileDiscarded the tile discarded by the other player
     */
    void getOppDiscard(Tile tileDiscarded) override
    {
        this->knowntile[tileDiscarded.toInt()]++;
        // TODO: Complete this function

    }

    /**
     * @brief this method will be called in Game when you can claim for a winning
     * @param winningTile The tile that is going to make your hand a winning hand
     * @param isSelfDraw whether the type of winning is Self Draw
     * @return whether you agree to claim for a winning
     */
    bool requestWin(Tile winningTile, bool isSelfDraw) override
    {

        // TODO: Complete this function (or you can just leave it "return true")

        return true;
    }

};

