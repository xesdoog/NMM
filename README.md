# No Man's Menu

![Screenshot 2025-10-03 221332](https://github.com/user-attachments/assets/5d4332de-e7aa-45c0-b444-c21731e2d8b4)

___

## Features

| 🧑‍🚀 Player | 🔫 Multitool | 🚀 Starship |
| ----------- | ----------- | ---------- |
| Infinite Health | Infinite Mining Beam | Infinite Shield |
| Infinite Stamina | Infinite Terrain Manipulator | Infinite Launch Thrusters |
| Infinite Jetpack | Infinite Ammo | Infinite Pulse Engine |
| Infinite Life Support | Infinite Grenades | Infinite Hyperdrive |
| Infinite Environmental Protection | One Hit Kill | No Weapon Overheat (lasers & rockets) |
| Infinite Exosuit Shields | No Mining Beam Overheat | Unlock All Corvette Parts |
| Free Crafting |  |  |
| Set Currency (Units, Nanites, Quicksilver) |  |  |

## Usage

- Compile the project *(will provide a pre-compiled binary at some point in the future)*. It will produce a binary: `NoMansMenu.dll`.
- Start the game, preferably load into a save as well.
- Use any injector and inject the DLL into `NMS.exe`.
- Press `INSERT` to open/close the menu.

## Credits

- [Blind Distortion](https://fearlessrevolution.com/memberlist.php?mode=viewprofile&u=107792&sid=6e14b7672f349c049f08797d36849609), the author of this [CheatEngine table](https://fearlessrevolution.com/viewtopic.php?t=30442) and contributors (BigJit, YoucefHam, the notorious gir489, ...).
- [Yimura](https://github.com/Yimura), the author of the template this repository was generated from.
- [Stackoverflow](https://stackoverflow.com) because I'm dumb.

### Known Issues

- **First ever** injection tanks your CPU 🥴 After that, patterns get cached and the menu loads instantly.
- Sometimes, initial injection fails with `Invalid Vulkan Device` error. Restarting the game and re-injecting the DLL works. Not sure how to fix this.

### TODO

- [ ] Add more stuff and hopefully make sense of some of this game's huge classes (Im not a reverse-engineer. [HELP](https://www.youtube.com/watch?v=zw8TCw42hZo)).
- [ ] Add a GitHub workflow to compile the project and upload a binary.
- [ ] Add serializable settings *(checkbox states, maybe themes and UI stuff?)*.
