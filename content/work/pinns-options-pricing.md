---
title: "PINNs Options Pricing"
date: 2025-06-01
category: "Finance"
summary: "Pricing European options with Physics-Informed Neural Networks that enforce the Black-Scholes PDE as a hard constraint."
github: "https://github.com/AlvaroMartinRuiz/PINNs_Options_Pricing"
stack: ["Python", "TensorFlow", "PINNs", "Black-Scholes"]
weight: 2
---

## Overview

Classical option pricing solves the Black–Scholes partial differential equation analytically or on finite-difference grids. This project asks a different question: can a neural network *learn* the price surface while being forced to obey the underlying financial physics?

Physics-Informed Neural Networks (PINNs) do exactly that — instead of fitting data alone, the network is penalised whenever it violates the governing PDE.

## How it works

- The network maps `(S, t)` — spot price and time — to the option value `V(S, t)`.
- The training loss combines three terms:
  - **PDE residual** — the Black–Scholes operator applied to the network's output must equal zero across the domain, enforced via automatic differentiation.
  - **Terminal condition** — at maturity, `V(S, T)` must equal the option payoff.
  - **Boundary conditions** — the expected behaviour as `S → 0` and `S → ∞`.
- Because the PDE is baked into the loss, the model stays consistent with financial theory and generalises from very few labelled points.

## Highlights

- End-to-end implementation in TensorFlow, using automatic differentiation to compute the PDE residual.
- Recovers the analytical Black–Scholes price for European calls and puts.
- A theory-first bridge between deep learning and quantitative finance.
