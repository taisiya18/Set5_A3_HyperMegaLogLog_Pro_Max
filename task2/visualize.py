#!/usr/bin/env python3
"""
Визуализация результатов HyperLogLog для этапа 2
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Настройка стиля графиков
plt.style.use('seaborn-v0_8-darkgrid')
plt.rcParams['figure.figsize'] = (14, 6)
plt.rcParams['font.size'] = 10

# Чтение данных
stats_df = pd.read_csv('statistics.csv')
single_df = pd.read_csv('single_stream.csv')

# График №1: Сравнение оценки Nt и Ft0 (один поток)
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))

# График 1a: Абсолютные значения
ax1.plot(single_df['percentage'], single_df['exact_count'], 
         'o-', linewidth=2, markersize=8, label='Точное значение F₀ᵗ', color='#2E86AB')
ax1.plot(single_df['percentage'], single_df['hll_estimate'], 
         's-', linewidth=2, markersize=8, label='Оценка HLL Nₜ', color='#A23B72')

ax1.set_xlabel('Процент обработанного потока (%)', fontsize=12, fontweight='bold')
ax1.set_ylabel('Количество уникальных элементов', fontsize=12, fontweight='bold')
ax1.set_title('График №1a: Сравнение точного значения и оценки HyperLogLog', 
              fontsize=14, fontweight='bold', pad=20)
ax1.legend(fontsize=11, loc='upper left')
ax1.grid(True, alpha=0.3)
ax1.set_xlim(0, 105)

# Добавляем аннотации для нескольких точек
for i in [0, 4, 9]:  # 10%, 50%, 100%
    exact = single_df.iloc[i]['exact_count']
    estimate = single_df.iloc[i]['hll_estimate']
    pct = single_df.iloc[i]['percentage']
    error = abs(estimate - exact) / exact * 100
    
    ax1.annotate(f'{error:.1f}% ошибка', 
                xy=(pct, estimate), 
                xytext=(10, -20), 
                textcoords='offset points',
                fontsize=9,
                bbox=dict(boxstyle='round,pad=0.5', facecolor='yellow', alpha=0.3),
                arrowprops=dict(arrowstyle='->', connectionstyle='arc3,rad=0'))

# График 1b: Относительная ошибка
ax2.plot(single_df['percentage'], single_df['error'] * 100, 
         'D-', linewidth=2, markersize=7, label='Относительная ошибка', color='#F18F01')
ax2.axhline(y=0.815, color='green', linestyle='--', linewidth=1.5, 
            label='Теоретическая граница 1.04/√m (0.82%)')
ax2.axhline(y=1.03, color='red', linestyle='--', linewidth=1.5, 
            label='Теоретическая граница 1.32/√m (1.03%)')

ax2.set_xlabel('Процент обработанного потока (%)', fontsize=12, fontweight='bold')
ax2.set_ylabel('Относительная ошибка (%)', fontsize=12, fontweight='bold')
ax2.set_title('График №1b: Относительная ошибка HyperLogLog', 
              fontsize=14, fontweight='bold', pad=20)
ax2.legend(fontsize=10, loc='upper right')
ax2.grid(True, alpha=0.3)
ax2.set_xlim(0, 105)
ax2.set_ylim(0, max(single_df['error'] * 100) * 1.2)

plt.tight_layout()
plt.savefig('graph1_comparison.png', dpi=300, bbox_inches='tight')
print("График №1 сохранен: graph1_comparison.png")

# График №2: Статистики оценки (среднее по всем потокам)
fig, ax = plt.subplots(figsize=(14, 8))

# Точное значение
ax.plot(stats_df['percentage'], stats_df['exact'], 
        'o-', linewidth=2.5, markersize=9, label='Точное значение F₀ᵗ', 
        color='#2E86AB', zorder=3)

# Среднее значение оценки
ax.plot(stats_df['percentage'], stats_df['mean_estimate'], 
        's-', linewidth=2.5, markersize=9, label='E(Nₜ) - среднее значение оценки', 
        color='#A23B72', zorder=3)

# Область неопределенности (E(Nt) ± σ)
ax.fill_between(stats_df['percentage'], 
                stats_df['mean_estimate'] - stats_df['std_dev'],
                stats_df['mean_estimate'] + stats_df['std_dev'],
                alpha=0.3, color='#A23B72', 
                label='Область неопределенности E(Nₜ) ± σ')

ax.set_xlabel('Процент обработанного потока (%)', fontsize=13, fontweight='bold')
ax.set_ylabel('Количество уникальных элементов', fontsize=13, fontweight='bold')
ax.set_title('График №2: Статистики оценки HyperLogLog по 10 потокам\n' + 
             'Среднее значение и стандартное отклонение', 
             fontsize=15, fontweight='bold', pad=20)
ax.legend(fontsize=12, loc='upper left')
ax.grid(True, alpha=0.3)
ax.set_xlim(0, 105)

# Добавляем текстовую информацию
textstr = f'Параметры:\nB = 14 (16384 регистра)\nКоличество потоков: 10\n' + \
          f'Размер потока: 1,000,000\n\nТеоретическая ошибка:\n' + \
          f'1.04/√m = 0.82%\n1.32/√m = 1.03%'
props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
ax.text(0.02, 0.98, textstr, transform=ax.transAxes, fontsize=10,
        verticalalignment='top', bbox=props)

plt.tight_layout()
plt.savefig('graph2_statistics.png', dpi=300, bbox_inches='tight')
print("График №2 сохранен: graph2_statistics.png")

# Дополнительный график: Сравнение точности
fig, ax = plt.subplots(figsize=(14, 6))

mean_errors = stats_df['mean_error'] * 100
percentages = stats_df['percentage']

bars = ax.bar(percentages, mean_errors, width=7, alpha=0.7, 
              color='#F18F01', edgecolor='black', linewidth=1.5)

# Теоретические границы
ax.axhline(y=0.815, color='green', linestyle='--', linewidth=2, 
           label='Теоретическая граница 1.04/√m = 0.82%', zorder=3)
ax.axhline(y=1.03, color='red', linestyle='--', linewidth=2, 
           label='Теоретическая граница 1.32/√m = 1.03%', zorder=3)

# Подписи значений на столбцах
for i, (pct, err) in enumerate(zip(percentages, mean_errors)):
    ax.text(pct, err + 0.05, f'{err:.2f}%', ha='center', va='bottom', 
            fontsize=9, fontweight='bold')

ax.set_xlabel('Процент обработанного потока (%)', fontsize=13, fontweight='bold')
ax.set_ylabel('Средняя относительная ошибка (%)', fontsize=13, fontweight='bold')
ax.set_title('График №3: Средняя точность HyperLogLog на разных этапах потока', 
             fontsize=15, fontweight='bold', pad=20)
ax.legend(fontsize=11, loc='upper right')
ax.grid(True, alpha=0.3, axis='y')
ax.set_xlim(0, 110)

plt.tight_layout()
plt.savefig('graph3_accuracy.png', dpi=300, bbox_inches='tight')
print("График №3 сохранен: graph3_accuracy.png")

# Таблица со статистикой
print("\n" + "="*80)
print("СТАТИСТИКА ТОЧНОСТИ HYPERLOGLOG (B=14)")
print("="*80)
print(f"{'Шаг':<8} {'Точное F₀':<12} {'E(Nₜ)':<12} {'σ(Nₜ)':<12} {'Ошибка %':<12}")
print("-"*80)

for idx, row in stats_df.iterrows():
    print(f"{row['percentage']:>6.0f}%  {row['exact']:<12.0f} "
          f"{row['mean_estimate']:<12.0f} {row['std_dev']:<12.0f} "
          f"{row['mean_error']*100:>10.2f}%")

print("="*80)
print(f"\nСредняя ошибка по всем шагам: {stats_df['mean_error'].mean()*100:.2f}%")
print(f"Максимальная ошибка: {stats_df['mean_error'].max()*100:.2f}%")
print(f"Минимальная ошибка: {stats_df['mean_error'].min()*100:.2f}%")
print(f"\nВсе ошибки в пределах теоретической границы 1.32/√m: " +
      ("ДА ✓" if stats_df['mean_error'].max() < 0.0103 else "НЕТ ✗"))

plt.show()
