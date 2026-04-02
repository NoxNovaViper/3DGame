#include "combat/HealingPotion.h"
#include "entities/Player.h"

HealingPotion::HealingPotion() : Skill("Healing Potion", 50, 0) {}

void HealingPotion::use(Entity* target) {
    if (target != nullptr) {
        Player* player = dynamic_cast<Player*>(target);
        if (player) {
            player->Heal(amount);
        }
    }
}
