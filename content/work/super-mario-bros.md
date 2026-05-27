---
title: "Super Mario Bros"
date: 2021-11-01
category: "Game Development"
summary: "A 2D Super Mario Bros platformer built from scratch in Python with a custom physics engine and enemy AI."
github: "https://github.com/AlvaroMartinRuiz/super_mario_bross"
stack: ["Python", "Pygame", "OOP"]
weight: 3
---

## Overview

A from-scratch recreation of the classic 2D platformer, written in Python with Pygame. The goal was to build a real game loop — physics, collisions, enemies and state — using clean object-oriented design rather than a ready-made engine.

## How it works

- **Custom physics** — gravity, jumping and collision resolution handled by hand.
- **Enemy AI** — Goomba and Koopa Troopa behaviours, including patrol and stomp interactions.
- **Power-up states** — multiple Mario states with the transitions between them.
- **OOP architecture** — sprites, levels and entities modelled as well-separated classes.

## Run it yourself

```bash
git clone https://github.com/AlvaroMartinRuiz/super_mario_bross
cd super_mario_bross
python main.py
```
