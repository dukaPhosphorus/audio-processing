import numpy
import pyaudio
import scipy.io.wavfile as scw
import scipy.signal as sg


def load_elev0hrtf():
    elev0Hrtf_L = {}
    elev0Hrtf_R = {}
    
    for i in range(72):
        str_i = str(i * 5)
        
        if len(str_i) < 2:
            str_i = "00" + str_i
        elif len(str_i) < 3:e
        fileName = "L0e" + str_i + "a.dat"
        filePath = "./hrtfs/elev0/" + fileName
        test = open(filePath, "r").read().split("\n")
        
        data = []
        
        for item in test:
            if item != '':
                data.append(float(item))
        
        elev0Hrtf_L[i] = data

    for i in range(72):
        str_i = str(i * 5)

        if len(str_i) < 2:
            str_i = "00" + str_i
        elif len(str_i) < 3:
            str_i = "0" + str_i

        fileName = "R0e" + str_i + "a.dat"
        filePath = "./hrtfs/elev0/" + fileName
        test = open(filePath, "r").read().split("\n")

        data = []

        for item in test:
            if item != '':
                data.append(float(item))

        elev0Hrtf_R[i] = data

    return elev0Hrtf_L, elev0Hrtf_R

def convolution(data, hrtf, N, L):
    spectrum = numpy.fft.fft(data, n = N)
    hrtf_fft = numpy.fft.fft(hrtf, n = N)
    add = spectrum * hrtf_fft
    result = numpy.fft.ifft(add, n = N)
    return_data = result.real
    return return_data[:L], return_data[L:]

def play_elev0(sound_data, N, L, hrtfL, hrtfR, position, overLap, streamObj):
    index = 0
    overLap_L = numpy.zeros(overLap)
    overLap_R = numpy.zeros(overLap)

    global rate


    while(sound_data[index:].size > L):
        result_data = numpy.empty((0, 2), dtype=numpy.int16)
        t=index/rate
        position=t*360/10


        pos_index=int(((position/360.0)%1.0)*72)
        tmp_conv_R, add_R = convolution(sound_data[index:index + L, 1], hrtfR[pos_index], N, L)
        tmp_conv_L, add_L = convolution(sound_data[index:index + L, 0], hrtfL[pos_index], N, L)


        tmp_conv_L[:overLap] += overLap_L
        tmp_conv_R[:overLap] += overLap_R

        overLap_L = add_L
        overLap_R = add_R
        for i in range(tmp_conv_L.size):
            result_data = numpy.append(result_data, numpy.array([[int(tmp_conv_L[i]), int(tmp_conv_R[i])]], dtype=numpy.int16), axis=0)

        streamObj.write(bytes(result_data))
        index += L
        
        


    streamObj.close()


#初期設定など
soundDataPath = "./adventurers.WAV"
rate, soundData = scw.read(soundDataPath)

p = pyaudio.PyAudio()
stream = p.open(format = 8,
                channels = 2,
                rate = rate,
                output = True)

hrtf_L, hrtf_R = load_elev0hrtf()
N = 1024
L = 513
overLap = 511
#音の再生する方向(0が正面で半時計回りで71まで)
position = 0

play_elev0(soundData, N, L, hrtf_L, hrtf_R, position, overLap, stream)
p.terminate()