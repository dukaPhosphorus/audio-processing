#!/bin/csh

# プロジェクトディレクトリ (各自の作業ディレクトリ名に置き換えてください)
set prjdir="~/Desktop/proen/proen_VC"

# 結果保存用ディレクトリ
set out="out"

# 変換元 -> src, 変換先 -> tgt
# に設定してください
set src="kozuka"
set tgt="matsumura"

# 音声特徴量抽出用パラメータ
set sr=16000 # サンプリング周波数 (16000[Hz]で固定)
set shiftl=5 # フレームシフト長 (5[msec]で固定)
# (perl/make_f0hist.pl の実行結果を参考にして設定)
set minF0_src=100
set maxF0_src=350
set minF0_tgt=50
set maxF0_tgt=230

set ndim_mgc=39 # メルケプストラム係数の次数
set ndim_bap=5 # 平均非周期性指標の次数

# DNNの設定
set gpu=-1 # GPUを使うか使わないか
set batch=128 # バッチサイズ
set epoch=25 # エポック数
set nhid=256 # 隠れユニット数
set lr=0.01 # 学習率

# スクリプト類
set EXT_FEATS="perl perl/FeatExtract_one.pl" # 音声特徴量抽出
set MAKE_DATA="python python/make_data.py" # 学習データ作成 & メルケプ・F0の平均と分散を計算
set TRAIN="python python/train.py" # 音声変換モデル学習
set PREDICT="python python/predict_gvpf.py" # 学習済みモデルを用いた音声特徴量変換
set SYNTHESIS="sh syn.sh" # 音声波形生成

# 動作フラグ
set EXTFS=0 # 音声特徴量抽出 (変換元話者)
set EXTFT=0 # 音声特徴量抽出 (変換先話者)
set MKDAT=1 # 学習データ・テストデータ作成
set TRGEN=1 # 音声変換モデル学習
set PRGEN=1 # 音声特徴量変換
set SYNTH=1 # 音声波形生成

# 音声特徴量抽出 (変換元話者)
set opt="${prjdir} ${src} ${sr} ${minF0_src} ${maxF0_src} ${shiftl} ${ndim_mgc} "
if (${EXTFS} == 1) then
  ${EXT_FEATS} ${opt}
endif

# 音声特徴量抽出 (変換先話者)
set opt="${prjdir} ${tgt} ${sr} ${minF0_tgt} ${maxF0_tgt} ${shiftl} ${ndim_mgc} "
if (${EXTFT} == 1) then
  ${EXT_FEATS} ${opt}
endif

# 学習データ・テストデータ作成
set opt=" --prjdir ${prjdir} --dim ${ndim_mgc} "
if (${MKDAT} == 1) then
  ${MAKE_DATA} ${opt} ${src} ${tgt}
endif

# 音声変換モデル学習
set opt=" --prjdir ${prjdir} --epoch ${epoch} --gpu ${gpu} --batch ${batch} --nhid ${nhid} --lr ${lr} --ndim_mgc ${ndim_mgc} "
if (${TRGEN} == 1) then
  ${TRAIN} ${opt} ${src} ${tgt} ${out}
endif

# 音声特徴量抽出
set opt=" --prjdir ${prjdir} --gpu ${gpu} --nhid ${nhid} --ndim_mgc ${ndim_mgc} --ndim_bap ${ndim_bap} "
if (${PRGEN} == 1) then
  ${PREDICT} ${opt} ${src} ${tgt} ${out}
endif

# 音声波形生成
set opt=" ${out} ${ndim_mgc} "
if (${SYNTH} == 1) then
  ${SYNTHESIS} ${opt}
endif
