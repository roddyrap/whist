# Whist

## Installation

The game will hopefully be available on the Arch user repository (AUR) soon.

### From Source

- Clone the git repository to your PC.
- Run `mkdir -p build/`
- Enter the build directory.
- Run `cmake ..`
- Run `cmake --build .`
- The terminal program will be available at `build/Terminal/WhistTerminal`

## Game instructions

Whist is a card game about not just being to play what you got in the best way but also expecting it. The game enables you to make the most out of what you got regardless of it being "weak" or "strong".

### Game Setup

You can only play the game with exactly four players. The entire card deck should be dealt, and each player should end up with thirteen cards in their hand.
Takes - A game round
The taking mechanism is the core gameplay loop of the game. Each take consists of a single round. The first player chooses any card to play from their hand. Other players must then play a card of the same suit from their hand. After every player has finished their turn, the player that played the highest card gains a take and can start the next round.

#### Example

| Player | Card Played |
|--------|-------------|
|      1 |          ♥Q |
|      2 |          ♥2 |
|      3 |          ♥A |
|      4 |          ♥7 |

Player 3  put the highest valid card in the round. As such, they won the round and got an additional take.

### Betting

The player's bet is their guess on how well they'll do this round, according to the hand they have
drawn.

#### Initial Betting

The initial betting round is used to determine the ruling player and the ruling suit. To bet in it a player must bet on at least 5 takes or more. Only the highest bet is considered, and betting continues until all players but one choose to skip. If multiple players chose to bet the same number, the highest bet is picked according to the following type order (Strongest -> Weakest): No Suit, Spades, Hearts, Diamonds, Clubs.

The player which made the highest bet then becomes the ruling player, and the suit in their bet becomes the ruling suit.

#### Secondary Betting

The secondary betting round is when the players that aren't ruling complete their bets and starts from the player after the ruling player. Every player can choose to bet any number they wish (from 0 to 13). The only constraint is that the last player's bet must not complete the other players' bets to thirteen.

### Cutting

If a player doesn't have any cards of the current round's suit, they are not bound to it and can choose to play any card they wish. If they play any other card then their card is invalid and even if it's higher than all other cards it will not be able to win the round and get a take.

However, if a player chooses to play a card of the ruling suit, their card automatically takes precedence over all other cards of the previous suit unless they have already been of the ruling suit. They are then guaranteed to win the round only unless someone else is also cutting with a higher card of the ruling suit.

### Invalid Game

The game can become invalid if all players skipped in the initial betting round. There will be no ruling suit and no ruling player, and the game will not be playable.

There are three options to handle an invalid game:
Continuing without a suit. Letting the first player start the game.
Restarting the game and shuffling the cards.
Ending the game.

### Calculating Points

A player gets 10 points for every take they bet they'd make and lose 10 points for every take they bet they would take and failed and for every bet they didn't bet they'd make.

Formula: $10\Big(\min\limits_{takes \dots bets} -  |takes - bets|\Big)$

#### Calculating Points for Zero

When betting zero the amount of points received depends on the sum of bets in the game. If the sum of bets is greater than 13, the betting bonus is 50 and if it's less than 13 the betting bonus is 100.\

If the better didn't take at all, they earn their bonus as points. If they did, they earn ten points for each take and negate the bonus from it. This means that if a player that bet zero failed their bet they have a motive to take as much as they possibly can.

***

### Game Walkthrough

(The hands were dealt randomally and I am honestly shocked by how good player 2's hand is.)\
Player 1's hand: ♦2 ♦7 ♦9 ♦A ♣2 ♣5 ♣6 ♣J ♥4 ♥9 ♠3 ♠Q ♠A\
Player 2's hand: ♦3 ♦4 ♣3 ♣Q ♣K ♥5 ♥10 ♥J ♥Q ♥K ♥A ♠4 ♠7\
Player 3's hand: ♦5 ♦8 ♦10 ♦J ♦K ♣8 ♣9 ♥2 ♥3 ♥7 ♠2 ♠9 ♠K\
Player 4's hand: ♦6 ♦Q ♣4 ♣7 ♣10 ♣A ♥6 ♥8 ♠5 ♠6 ♠8 ♠10 ♠J

#### Initial Bets

- Player 1 estimates that they could at best have 4 takes, which is less than 5, so they skip.\
  Estimation breakdown: Player 1 thinks they are likely to take with their AD, ♠Q or ♠A. If need be they could also try to take with ♣J but even that doesn't reach five, so they skip.

- Player 2 estimates that due to their incredible hand their strongest suit is ♥ and they bet 7.\
  Estimation breakdown: Player 2 has been randomally dealt one of the best hands I have ever seen. If hearts becomes the ruling suit they are guaranteed at least 4 takes (A - J H are the strongest cards in the game) and will probably take with their 5 as it's a ruling card. They can also probably take with their ♣Q or ♣K so they bet 7.

- Player 3 estimates that they could at best have 2 takes, which is less than 5, so they skip.\
  Estimation breakdown: Player 3 doesn't have 5 enough royal cards, so their chances of taking 5 of any suit are minimal.

- Player 4 estimates that they could at best have 3 takes, which is less than 5, so they skip.\
  Estimation breakdown: Player 4 doesn't have 5 enough royal cards, so their chances of taking 5 of any suit are minimal.

Player 3 is the only that bet, so they become the ruling player and ♥ is the game's ruling suit.

#### Secondary Bets

- Player 1 bets 3.\
  Bet breakdown: Player 1 doesn't have many strong hearts, but they feel confident that their ♠A ♠Q and ♦A are strong enough.

- Player 2 can't change their bet due to them being the ruling player, so they bet 7.

- Player 3 bets 2.\
  Bet breakdown: Player 3 thinks they will only take with their kings, ♠K and ♦K, so they bet 2. If things come to worst, they have ♦J so with their strong cards they feel pretty safe.

- Player 4 wanted to bet 1 but that's invalid as it would complete the bets to 13 (3 + 7 + 2 + 1 = 13) so they bet 2.\
  Bet breakdown: Player 4 has a very weak hand. They will probably take with their ♣A but they don't have many options for another take. Unfortunately, they can't bet 1. Considering their options, player 4 decided to bet 2 and not 0 because they figure it will be pretty hard not to take with their ace. Being the last player, this puts player 4 in a pretty poor state.

#### Takes

| Round Number | Starting Player | Round Type | Player 1 | Player 2 | Player 3 | Player 4 | Round Winner | Comments |
|--------------|-----------------|------------|----------|----------|----------|----------|--------------|----------|
|            1 |               2 |      ♥     |    ♥4    |     ♥A   |    ♥2    |    ♥6    |       2      | The sum of bets is over 13, meaning that if all players estimated well there aren't enough takes. Because of this, player 2 wants to maximise their takes and start strong. |
|            2 |               2 |      ♥     |    ♥9    |     ♥K   |    ♥3    |    ♥8    |       2      |          |
|            3 |               2 |      ♥     |    ♦2    |     ♥Q   |    ♥7    |    ♣4    |       2      | Player 4 and Player 1 don't have any more ♥ cards, so they are forced to dump other cards. |
|            4 |               2 |      ♥     |    ♣2    |     ♥J   |    ♠2    |    ♠5    |       2      | Player 3 also runs out of hearts. |
|            5 |               2 |      ♥     |    ♠3    |    ♥10   |    ♦5    |    ♦6    |       2      | At this point player 2 notices that all of the other players don't have hearts, meaning ♥5 is also a guarenteed take. |
|            6 |               2 |      ♥     |    ♣5    |     ♥5   |    ♣8    |    ♠6    |       2      |          |
|            7 |               2 |      ♥     |    ♣J    |     ♣Q   |    ♣9    |    ♣A    |       4      | At this point player 2 doesn't care as much if they win. They try to bait another player to show their ♣A so they could take with the queen in the next ♣ round. Player 4 sees this oppurtunity and uses the ♣A. |
|            8 |               4 |      ♠     |    ♠A    |     ♠4   |    ♠9    |    ♠8    |       1      | Player 4 didn't expect the game to favor hearts this much, so they are stuck. They don't have a sure way to take their second bet and they don't have enough cards to bait another player. They throw a round hoping for a better oppurtunity later in the game. |
|            9 |               1 |      ♦     |    ♦A    |     ♦3   |    ♦8    |    ♦Q    |       1      | Player 1 is still playing strong hoping to get more takes. Player 4 realizes that they will probably not succeed to fulfil their bet. |
|           10 |               1 |      ♣     |    ♣6    |     ♣K   |   ♦10    |    ♣7    |       2      | Player 1 knows that the ♠K hasn't been played yet, so they throw this round hoping that it will be thrown away later. Player 3 knows that if a diamond round comes they have too many strong diamond cards, so they throw one away. |
|           11 |               2 |      ♣     |    ♦7    |     ♣3   |    ♦J    |   ♣10    |       4      | Player 2 completed their bet, and as such they don't want to take any more. Player 4 has luckily won with a ♣10! |
|           12 |               4 |      ♠     |    ♠Q    |     ♠7   |    ♠K    |    ♠J    |       3      | Player 4 has completed their bet. They don't have much choice, but they know ♠Q and ♠K have't been played yet so they hope to bait them out. |
|           13 |               3 |      ♦     |    ♦9    |     ♦4   |    ♦K    |   ♠10    |       3      | Just in the end, player 3 has completed their bet. |

#### Final standings

| Player | Takes | Bets | Points |
|--------|-------|------|--------|
|    1   |    2  |   3  |   10   |
|    2   |    7  |   7  |   70   |
|    3   |    2  |   2  |   20   |
|    4   |    2  |   2  |   20   |

***

## Plans for the future

- Add more customization options: Terminal multiplayer, more cards.
- Add LAN play.
- Add a graphical UI option.

## Known bugs
