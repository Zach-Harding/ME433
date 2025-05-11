import csv
import matplotlib.pyplot as plt
import numpy as np

hamming = [
    -0.000604064461847955,
    -0.000551780069321844,
    -0.000507919670677880,
    -0.000454487696572236,
    -0.000369375235857037,
    -0.000227140994277224,
    0.000000000000000000,
    0.000341015939334090,
    0.000824770783395443,
    0.001478661559683630,
    0.002327237052419366,
    0.003390857033446438,
    0.004684451187085917,
    0.006216434312797693,
    0.007987829321982980,
    0.009991642156324147,
    0.012212523327184123,
    0.014626739684373311,
    0.017202467721139997,
    0.019900406721944881,
    0.022674696908148821,
    0.025474114994420292,
    0.028243507783721956,
    0.030925414113290178,
    0.033461817071338058,
    0.035795962308429512,
    0.037874174746726358,
    0.039647605214021407,
    0.041073840550354880,
    0.042118315486222989,
    0.042755471888911503,
    0.042969620523712268,
    0.042755471888911503,
    0.042118315486222989,
    0.041073840550354880,
    0.039647605214021414,
    0.037874174746726358,
    0.035795962308429519,
    0.033461817071338058,
    0.030925414113290181,
    0.028243507783721956,
    0.025474114994420295,
    0.022674696908148818,
    0.019900406721944885,
    0.017202467721140008,
    0.014626739684373313,
    0.012212523327184133,
    0.009991642156324143,
    0.007987829321982982,
    0.006216434312797696,
    0.004684451187085917,
    0.003390857033446442,
    0.002327237052419366,
    0.001478661559683631,
    0.000824770783395443,
    0.000341015939334090,
    0.000000000000000000,
    -0.000227140994277224,
    -0.000369375235857037,
    -0.000454487696572237,
    -0.000507919670677880,
    -0.000551780069321844,
    -0.000604064461847955,
]

blackman = [
    0.000000000000000000,
    -0.000011466433343440,
    -0.000048159680438716,
    -0.000070951498745996,
    0.000000000000000000,
    0.000226394896924650,
    0.000570593070542985,
    0.000843882357908127,
    0.000742644459879189,
    -0.000000000000000001,
    -0.001387330856962112,
    -0.002974017320060804,
    -0.003876072294410999,
    -0.003078546062261788,
    0.000000000000000002,
    0.004911281747189231,
    0.009897689392343489,
    0.012239432700612913,
    0.009302643950572093,
    -0.000000000000000005,
    -0.013947257358995015,
    -0.027649479941983943,
    -0.034045985830080311,
    -0.026173578588735643,
    0.000000000000000007,
    0.043288592274982135,
    0.096612134128292462,
    0.148460098539443669,
    0.186178664190551207,
    0.199977588313552862,
    0.186178664190551235,
    0.148460098539443669,
    0.096612134128292462,
    0.043288592274982128,
    0.000000000000000007,
    -0.026173578588735653,
    -0.034045985830080325,
    -0.027649479941983936,
    -0.013947257358995019,
    -0.000000000000000005,
    0.009302643950572094,
    0.012239432700612920,
    0.009897689392343489,
    0.004911281747189235,
    0.000000000000000002,
    -0.003078546062261790,
    -0.003876072294411004,
    -0.002974017320060808,
    -0.001387330856962112,
    -0.000000000000000001,
    0.000742644459879189,
    0.000843882357908127,
    0.000570593070542984,
    0.000226394896924650,
    0.000000000000000000,
    -0.000070951498745996,
    -0.000048159680438716,
    -0.000011466433343440,
    0.000000000000000000,
]

rectangular = [
    -0.028094547010435022,
    0.000000000000000007,
    0.042141820515652538,
    0.090915863922831200,
    0.136373795884246835,
    0.168567282062610097,
    0.180191569250188838,
    0.168567282062610097,
    0.136373795884246835,
    0.090915863922831200,
    0.042141820515652538,
    0.000000000000000007,
    -0.028094547010435022,
]

def MAF(signal, step):
    newsignal = []
    for i in range(len(signal)-step):
        newsignal.append(sum(signal[i:i+step])/step)
    return newsignal

def IIR(signal, a, b):
    newsignal = []
    newsignal.append(signal[0])
    for i in range(1, len(signal)):
        newsignal.append(a*newsignal[i-1] + b*signal[i])
    return newsignal

def FIR(signal, coeffs):
    newsignal = []
    for i in range(len(signal)):
        newsignal.append(0)
        for j in range(len(coeffs)):
            if i-j >= 0:
                newsignal[i] += coeffs[j] * signal[i-j]
    return newsignal


tA = []
sigA = []
tB = []
sigB = []
tC = []
sigC = []
tD = []
sigD = []

with open('sigA.csv') as file1:
    reader = csv.reader(file1)
    for row in reader:
        tA.append(float(row[0]))
        sigA.append(float(row[1]))
        
with open('sigB.csv') as file2:
    reader = csv.reader(file2)
    for row in reader:
        tB.append(float(row[0]))
        sigB.append(float(row[1]))

with open('sigC.csv') as file3:
    reader = csv.reader(file3)
    for row in reader:
        tC.append(float(row[0]))
        sigC.append(float(row[1]))

with open('sigD.csv') as file4:
    reader = csv.reader(file4)
    for row in reader:
        tD.append(float(row[0]))
        sigD.append(float(row[1]))


times = [tA, tB, tC, tD]
Ys = []
frqs = []


dt = 1.0/10000.0 # 10kHz
t = np.arange(0.0, 1.0, dt) # 10s
Fs = 10000 # sample rate
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector


mafA = MAF(sigA, 25)
tfa = np.linspace(tA[0], tA[-1], len(mafA))
mafB = MAF(sigB, 80)
tfb = np.linspace(tB[0], tB[-1], len(mafB))
mafC = MAF(sigC, 10)
tfc = np.linspace(tC[0], tC[-1], len(mafC))
mafD = MAF(sigD, 20)
tfd = np.linspace(tD[0], tD[-1], len(mafD))

iirA = IIR(sigA, 0.91, 0.09)
tia = np.linspace(tA[0], tA[-1], len(iirA))
iirB = IIR(sigB, 0.98, 0.02)
tib = np.linspace(tB[0], tB[-1], len(iirB))
iirC = IIR(sigC, 0.5, 0.5)
tic = np.linspace(tC[0], tC[-1], len(iirC))
iirD = IIR(sigD, 0.85, 0.15)
tid = np.linspace(tD[0], tD[-1], len(iirD))

firA = FIR(sigA, hamming)
tra = np.linspace(tA[0], tA[-1], len(firA))
firB = FIR(sigB, hamming)
trb = np.linspace(tB[0], tB[-1], len(firB))
firC = FIR(sigC, rectangular)
trc = np.linspace(tC[0], tC[-1], len(firC))
firD = FIR(sigD, blackman)
trd = np.linspace(tD[0], tD[-1], len(firD))







Signals = [sigA, sigB, sigC, sigD, mafA, mafB, mafC, mafD, iirA, iirB, iirC, iirD, firA, firB, firC, firD]

for signal in Signals:
    n = len(signal) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(signal)/n # fft computing and normalization
    Y = Y[range(int(n/2))]

    Ys.append(Y)
    frqs.append(frq)


#---------------------------------------------------------------------

# MAF SIGNAL A
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(tA,sigA, 'black')
ax1.plot(tfa,mafA, 'red')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title('MAF Signal A (window size 25)')
plt.legend(['Original', 'mafA'])

ax2.loglog(frqs[0],abs(Ys[0]), 'black') # plotting the fft
ax2.loglog(frqs[4],abs(Ys[4]), 'red') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.legend(['Original', 'mafA'])

# MAF SIGNAL B
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(tB,sigB, 'black')
ax1.plot(tfb,mafB, 'red')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title('MAF Signal B (window size 80)')
plt.legend(['Original', 'mafB'])

ax2.loglog(frqs[1],abs(Ys[1]), 'black') # plotting the fft
ax2.loglog(frqs[5],abs(Ys[5]), 'red') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.legend(['Original', 'mafB'])

# MAF SIGNAL C
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(tC,sigC, 'black')
ax1.plot(tfc,mafC, 'red')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title('MAF Signal C (window size 10)')
plt.legend(['Original', 'mafC'])

ax2.loglog(frqs[2],abs(Ys[2]), 'black') # plotting the fft
ax2.loglog(frqs[6],abs(Ys[6]), 'red') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.legend(['Original', 'mafC'])

# MAF SIGNAL D
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(tD,sigD, 'black')
ax1.plot(tfd,mafD, 'red')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title('MAF Signal D (window size 20)')
plt.legend(['Original', 'mafD'])

ax2.loglog(frqs[3],abs(Ys[3]), 'black') # plotting the fft
ax2.loglog(frqs[7],abs(Ys[7]), 'red') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.legend(['Original', 'mafD'])

# ------------------------------------------------------------------

# IIR SIGNAL A
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(tA,sigA, 'black')
ax1.plot(tia,iirA, 'red')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title('IIR Signal A (weights 0.91, 0.09)')
plt.legend(['Original', 'iirA'])

ax2.loglog(frqs[0],abs(Ys[0]), 'black') # plotting the fft
ax2.loglog(frqs[8],abs(Ys[8]), 'red') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.legend(['Original', 'iirA'])

# IIR SIGNAL B
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(tB,sigB, 'black')
ax1.plot(tib,iirB, 'red')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title('IIR Signal B (weights 0.98, 0.02)')
plt.legend(['Original', 'iirB'])

ax2.loglog(frqs[1],abs(Ys[1]), 'black') # plotting the fft
ax2.loglog(frqs[9],abs(Ys[9]), 'red') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.legend(['Original', 'iirB'])

# IIR SIGNAL C
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(tC,sigC, 'black')
ax1.plot(tic,iirC, 'red')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title('IIR Signal C (weights 0.5, 0.5)')
plt.legend(['Original', 'iirC'])

ax2.loglog(frqs[2],abs(Ys[2]), 'black') # plotting the fft
ax2.loglog(frqs[10],abs(Ys[10]), 'red') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.legend(['Original', 'iirC'])

# IIR SIGNAL D
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(tD,sigD, 'black')
ax1.plot(tid,iirD, 'red')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title('IIR Signal D (weights 0.85, 0.15)')
plt.legend(['Original', 'iirD'])

ax2.loglog(frqs[3],abs(Ys[3]), 'black') # plotting the fft
ax2.loglog(frqs[11],abs(Ys[11]), 'red') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.legend(['Original', 'iirD'])

# ------------------------------------------------------------------

# FIR SIGNAL A
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(tA,sigA, 'black')
ax1.plot(tra,firA, 'red')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title('FIR Signal A (Hamming 62 weights, cuttoff 200Hz, bandwidth 500Hz)')
plt.legend(['Original', 'firA'])

ax2.loglog(frqs[0],abs(Ys[0]), 'black') # plotting the fft
ax2.loglog(frqs[12],abs(Ys[12]), 'red') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.legend(['Original', 'firA'])

# FIR SIGNAL B
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(tB,sigB, 'black')
ax1.plot(trb,firB, 'red')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title('FIR Signal B (Hamming 62 weights, cuttoff 200Hz, bandwidth 500Hz)')
plt.legend(['Original', 'firB'])

ax2.loglog(frqs[1],abs(Ys[1]), 'black') # plotting the fft
ax2.loglog(frqs[13],abs(Ys[13]), 'red') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.legend(['Original', 'firB'])

# FIR SIGNAL C
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(tC,sigC, 'black')
ax1.plot(trc,firC, 'red')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title('FIR Signal C (Rectangular, 12 weights, cuttoff 1000Hz, bandwidth 800Hz)')
plt.legend(['Original', 'firC'])

ax2.loglog(frqs[2],abs(Ys[2]), 'black') # plotting the fft
ax2.loglog(frqs[14],abs(Ys[14]), 'red') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.legend(['Original', 'firC'])

# FIR SIGNAL D
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(tD,sigD, 'black')
ax1.plot(trd,firD, 'red')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.set_title('FIR Signal D (Blackman 58 weights, cuttoff 1000Hz, bandwidth 700Hz)')
plt.legend(['Original', 'firD'])

ax2.loglog(frqs[3],abs(Ys[3]), 'black') # plotting the fft
ax2.loglog(frqs[15],abs(Ys[15]), 'red') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.legend(['Original', 'firD'])





plt.show()
