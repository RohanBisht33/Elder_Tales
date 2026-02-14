class QLearningAgent {
    constructor() {
        this.qTable = {}; // Key: "playerHpBucket-aiHpBucket-action", Value: Q-value
        this.alpha = 0.1;
        this.gamma = 0.9;
        this.epsilon = 0.2;
    }

    getStateKey(playerHp, playerMax, aiHp, aiMax) {
        let pBucket = Math.floor((playerHp / playerMax) * 10);
        let aBucket = Math.floor((aiHp / aiMax) * 10);
        return `${pBucket}-${aBucket}`;
    }

    chooseAction(stateKey) {
        if (Math.random() < this.epsilon) {
            return Math.random() < 0.5 ? 'ATTACK' : 'HEAL';
        }

        const qAttack = this.qTable[`${stateKey}-ATTACK`] || 0;
        const qHeal = this.qTable[`${stateKey}-HEAL`] || 0;

        return qAttack >= qHeal ? 'ATTACK' : 'HEAL';
    }

    // Pre-train simply
    train() {
        // Simplified training loop placeholder
        // In a real app, we'd run simulation here
        console.log("AI Agent Initialized");
    }
}

class Game {
    constructor() {
        this.player = {
            name: "Hero",
            level: 1,
            hp: 200,
            maxHp: 200,
            gold: 1000,
            potions: 5,
            damage: 20
        };
        
        this.enemy = null;
        this.ai = new QLearningAgent();
        this.canvas = document.getElementById('gameCanvas');
        this.ctx = this.canvas.getContext('2d');
        this.logEl = document.getElementById('game-log');
        
        this.inBattle = false;
        this.loopId = null;

        // Stickman Animation State
        this.frame = 0;
        
        this.updateUI();
        this.startLoop();
        this.log("Game Loaded. Ready to explore.");
    }

    log(msg) {
        const p = document.createElement('p');
        p.textContent = `> ${msg}`;
        this.logEl.appendChild(p);
        this.logEl.scrollTop = this.logEl.scrollHeight;
    }

    updateUI() {
        document.getElementById('hp-val').textContent = Math.max(0, this.player.hp);
        document.getElementById('hp-max').textContent = this.player.maxHp;
        document.getElementById('level-val').textContent = this.player.level;
        document.getElementById('gold-val').textContent = this.player.gold;
        document.getElementById('potion-count').textContent = this.player.potions;

        if (this.inBattle) {
            document.getElementById('battle-ui').classList.remove('hidden');
            document.getElementById('exploration-ui').classList.add('hidden');
        } else {
            document.getElementById('battle-ui').classList.add('hidden');
            document.getElementById('exploration-ui').classList.remove('hidden');
        }
    }

    // --- Core Logic ---

    explore() {
        if (Math.random() < 0.5) {
            this.startBattle();
        } else {
            this.log("You explore the dark cave... nothing happens.");
            // Maybe find gold
            if (Math.random() < 0.3) {
                const found = Math.floor(Math.random() * 20) + 10;
                this.player.gold += found;
                this.log(`Found ${found} gold!`);
            }
            this.updateUI();
        }
    }

    rest() {
        this.player.hp = this.player.maxHp;
        this.log("You rested and recovered full HP.");
        this.updateUI();
    }

    startBattle() {
        this.inBattle = true;
        const type = Math.random() < 0.5 ? "Goblin" : (Math.random() < 0.5 ? "Orc" : "Slime");
        const hp = Math.floor(100 + (this.player.level * 20));
        
        this.enemy = {
            name: type,
            hp: hp,
            maxHp: hp,
            damage: 10 + (this.player.level * 2)
        };
        
        this.log(`Encountered a ${type}!`);
        this.updateUI();
    }

    playerAttack() {
        if (!this.enemy) return;
        
        // Crit chance
        let dmg = this.player.damage + Math.floor(Math.random() * 10);
        if (Math.random() < 0.2) dmg *= 1.5;
        dmg = Math.floor(dmg);

        this.enemy.hp -= dmg;
        this.log(`You hit ${this.enemy.name} for ${dmg} damage.`);
        
        // Animation trigger
        this.animateAttack('player');

        if (this.enemy.hp <= 0) {
            this.winBattle();
        } else {
            setTimeout(() => this.mnEnemyTurn(), 1000);
        }
        this.updateUI();
    }

    playerHeal() {
        if (this.player.potions > 0) {
            this.player.potions--;
            const heal = Math.floor(this.player.maxHp * 0.4);
            this.player.hp = Math.min(this.player.maxHp, this.player.hp + heal);
            this.log(`Used potion. Healed ${heal} HP.`);
            this.updateUI();
            setTimeout(() => this.mnEnemyTurn(), 1000);
        } else {
            this.log("No potions left!");
        }
    }

    playerFlee() {
        // Updated Flee Logic: Min 20%
        let chance = 0.2 + (this.player.hp / this.player.maxHp) * 0.3;
        if (Math.random() < chance) {
            this.log(`Escaped from ${this.enemy.name}!`);
            this.inBattle = false;
            this.enemy = null;
            this.updateUI();
        } else {
            this.log("Failed to flee!");
            setTimeout(() => this.mnEnemyTurn(), 1000);
        }
    }

    mnEnemyTurn() {
        if (!this.inBattle || !this.enemy) return;

        // AI Decide
        const stateKey = this.ai.getStateKey(this.player.hp, this.player.maxHp, this.enemy.hp, this.enemy.maxHp);
        const action = this.ai.chooseAction(stateKey);

        if (action === 'HEAL' && this.enemy.hp < this.enemy.maxHp * 0.5) {
            const heal = Math.floor(this.enemy.maxHp * 0.2);
            this.enemy.hp += heal;
            this.log(`${this.enemy.name} heals for ${heal} HP.`);
        } else {
            let dmg = this.enemy.damage + Math.floor(Math.random() * 5);
            
            // Evasion check
            if (Math.random() < 0.1) {
                this.log(`You evaded ${this.enemy.name}'s attack!`);
            } else {
                this.player.hp -= dmg;
                this.log(`${this.enemy.name} hits you for ${dmg} damage.`);
                this.animateAttack('enemy');
            }
        }

        if (this.player.hp <= 0) {
            this.log("You died... Game Over.");
            this.inBattle = false;
            this.player.hp = 0;
            // Reset logic could go here
        }
        
        this.updateUI();
    }

    winBattle() {
        this.log(`Defeated ${this.enemy.name}!`);
        this.player.gold += 50;
        this.player.level += 1;
        this.player.maxHp += 20;
        this.player.damage += 5;
        this.inBattle = false;
        this.enemy = null;
        this.updateUI();
    }

    // --- Drawing ---

    startLoop() {
        this.loopId = requestAnimationFrame(() => this.draw());
    }

    draw() {
        this.frame++;
        this.ctx.fillStyle = '#111';
        this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);

        // Draw Floor
        this.ctx.strokeStyle = '#333';
        this.ctx.lineWidth = 2;
        this.ctx.beginPath();
        this.ctx.moveTo(0, 300);
        this.ctx.lineTo(800, 300);
        this.ctx.stroke();

        // Draw Player Stickman
        this.drawStickman(150, 300, '#3b82f6', this.frame);

        // Draw Enemy Stickman if present
        if (this.inBattle && this.enemy) {
            this.drawStickman(650, 300, '#ef4444', this.frame + 20, true);
        }

        requestAnimationFrame(() => this.draw());
    }

    drawStickman(x, y, color, frame, isEnemy = false) {
        const ctx = this.ctx;
        ctx.strokeStyle = color;
        ctx.lineWidth = 3;
        ctx.lineCap = 'round';
        ctx.lineJoin = 'round';

        // Bounce effect
        const bounce = Math.sin(frame * 0.05) * 5;
        y -= bounce;

        // Head
        ctx.beginPath();
        ctx.arc(x, y - 60, 15, 0, Math.PI * 2);
        ctx.stroke();

        // Body
        ctx.beginPath();
        ctx.moveTo(x, y - 45);
        ctx.lineTo(x, y - 15);
        ctx.stroke();

        // Arms
        ctx.beginPath();
        ctx.moveTo(x, y - 35);
        if (isEnemy) {
            ctx.lineTo(x - 25, y - 25 + Math.sin(frame * 0.1) * 5); // Arm pointing left
        } else {
             ctx.lineTo(x + 25, y - 25 + Math.sin(frame * 0.1) * 5); // Arm pointing right
        }
        ctx.stroke();

        // Legs
        ctx.beginPath();
        ctx.moveTo(x, y - 15);
        ctx.lineTo(x - 15, y); // Left Leg
        ctx.moveTo(x, y - 15);
        ctx.lineTo(x + 15, y); // Right Leg
        ctx.stroke();
        
        // Sword
        if (!isEnemy) {
            ctx.strokeStyle = '#fff';
            ctx.beginPath();
            ctx.moveTo(x + 25, y - 25);
            ctx.lineTo(x + 55, y - 55);
            ctx.stroke();
        }
    }

    animateAttack(who) {
        // Simple visual shake or flash could be added here
        // For now, the stickman bobbing is continuous
    }
    
    toggleShop() {
        const modal = document.getElementById('shop-modal');
        if (modal.classList.contains('hidden')) {
            modal.classList.remove('hidden');
            this.renderShop();
        } else {
            modal.classList.add('hidden');
        }
    }
    
    renderShop() {
        const items = [
            {name: "Potion", price: 50, action: () => { 
                if (this.player.gold >= 50) {
                    this.player.gold -= 50;
                    this.player.potions++;
                    this.log("Bought Potion");
                    this.updateUI();
                }
            }}
        ];
        
        const container = document.getElementById('shop-items');
        container.innerHTML = '';
        items.forEach(item => {
            const div = document.createElement('div');
            div.className = 'shop-item';
            div.innerHTML = `<span>${item.name} (${item.price}g)</span> <button>Buy</button>`;
            div.querySelector('button').onclick = item.action;
            container.appendChild(div);
        });
    }
}

const game = new Game();
