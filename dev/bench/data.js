window.BENCHMARK_DATA = {
  "lastUpdate": 1776999071504,
  "repoUrl": "https://github.com/viktorika/jitfusion",
  "entries": {
    "JitFusion Engine Benchmark": [
      {
        "commit": {
          "author": {
            "email": "598855847@qq.com",
            "name": "Weiqiang Wu",
            "username": "viktorika"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "3780ab682c973d0b463866b60a4b80a190b78822",
          "message": "Merge pull request #221 from viktorika/dev\n\nfix benchmark",
          "timestamp": "2026-04-24T10:31:54+08:00",
          "tree_id": "cb39e1f1ac734ce0555b6031128e56953184661c",
          "url": "https://github.com/viktorika/jitfusion/commit/3780ab682c973d0b463866b60a4b80a190b78822"
        },
        "date": 1776999070687,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BM_Compile_MinimalExpr_mean",
            "value": 2.5107845603603383,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5106834630630632 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_MinimalExpr_median",
            "value": 2.495297756756888,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.4952838108108106 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_MinimalExpr_stddev",
            "value": 0.03650314058393758,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.036470826869201514 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_MinimalExpr_cv",
            "value": 0.014538539530726917,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.01452625446646575 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/10_mean",
            "value": 2.5049036954956425,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5048037117117117 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/10_median",
            "value": 2.5038538288290386,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.503833720720721 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/10_stddev",
            "value": 0.0037294589810390128,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0037189237186287852 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/10_cv",
            "value": 0.0014888632196700358,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0014847166271912695 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/100_mean",
            "value": 2.548533032727185,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.548221905454544 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/100_median",
            "value": 2.5470083454546386,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5469899909090907 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/100_stddev",
            "value": 0.006029700943903062,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.006157286085237539 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/100_cv",
            "value": 0.002365949692027605,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0024163068656060474 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/1000_mean",
            "value": 2.9948696297872948,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.9945731765957495 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/1000_median",
            "value": 2.988453680850906,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.9884331702127724 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/1000_stddev",
            "value": 0.013606119310213906,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.013679834349167419 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_LinearChain/1000_cv",
            "value": 0.0045431424376159765,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.004568208403148373 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/10_mean",
            "value": 2.5447146181818425,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.544361983636363 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/10_median",
            "value": 2.542836436363545,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5427235818181755 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/10_stddev",
            "value": 0.007161022280325166,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.006996866273154359 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/10_cv",
            "value": 0.0028140767648993197,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0027499492281969034 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/50_mean",
            "value": 2.710378548571433,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.7102241466666634 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/50_median",
            "value": 2.6794512380953552,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.6794289047618998 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/50_stddev",
            "value": 0.04921881201644716,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.04910268360517008 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/50_cv",
            "value": 0.01815938664449254,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.018117572919406702 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/200_mean",
            "value": 3.213796850000057,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3.2136308068181862 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/200_median",
            "value": 3.213399295454167,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 3.2133715454545353 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/200_stddev",
            "value": 0.0046710747883033,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.004649385880662026 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DeepNested/200_cv",
            "value": 0.0014534443234342016,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0014467703853216982 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/10_mean",
            "value": 2.530045027272598,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5299214181818193 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/10_median",
            "value": 2.5291357909090744,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.529042863636361 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/10_stddev",
            "value": 0.0032560157486133624,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.003236905610251932 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/10_cv",
            "value": 0.0012869398423803408,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0012794490718127528 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/100_mean",
            "value": 2.6759866457143198,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.675883718095242 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/100_median",
            "value": 2.676233133333231,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.6761158571428636 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/100_stddev",
            "value": 0.0055337423305213875,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.005562339850317111 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ManyFunctionCalls/100_cv",
            "value": 0.00206792598886241,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0020786926624287386 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ListOps_mean",
            "value": 2.7157402155338994,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.7156376970873723 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ListOps_median",
            "value": 2.715499533980544,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.7153736310679566 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ListOps_stddev",
            "value": 0.004928669110295236,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.004848066530376262 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_ListOps_cv",
            "value": 0.0018148529384745613,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0017852405479479106 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/2_mean",
            "value": 2.818987463999975,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.818892138000003 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/2_median",
            "value": 2.818140900000117,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.818035300000013 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/2_stddev",
            "value": 0.00322344462878061,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0032894661369245563 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/2_cv",
            "value": 0.0011434760423541348,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0011669357945914259 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/8_mean",
            "value": 4.533318899999842,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 4.5329170032257915 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/8_median",
            "value": 4.533265919354607,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 4.532133790322535 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/8_stddev",
            "value": 0.0023767115227281872,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.002527868147763696 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/8_cv",
            "value": 0.0005242762698049481,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0005576691887287527 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/32_mean",
            "value": 11.038855711999988,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 11.03817401599997 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/32_median",
            "value": 11.046643199999835,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 11.04563048000003 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/32_stddev",
            "value": 0.015998063784121153,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.01590402187267885 ms\nthreads: 1"
          },
          {
            "name": "BM_BatchCompile/32_cv",
            "value": 0.0014492501941782037,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0014408199988173562 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOn_mean",
            "value": 2.5337462522521808,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5336541081081005 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOn_median",
            "value": 2.5322710270269417,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5320443873873697 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOn_stddev",
            "value": 0.003130141625501585,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0031159984776937166 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOn_cv",
            "value": 0.0012353808605416914,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.001229843674289249 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOff_mean",
            "value": 2.516739076785614,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5165957196428477 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOff_median",
            "value": 2.5187146428576415,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 2.5186975178571442 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOff_stddev",
            "value": 0.003918939926895642,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.0038424256510533722 ms\nthreads: 1"
          },
          {
            "name": "BM_Compile_DumpIrOff_cv",
            "value": 0.0015571498702602582,
            "unit": "ms/iter",
            "extra": "iterations: 5\ncpu: 0.001526834692224099 ms\nthreads: 1"
          },
          {
            "name": "BM_Execute_ConstFold_mean",
            "value": 13.105463774362349,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.104854292850368 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ConstFold_median",
            "value": 12.846980156562987,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.846455071388794 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ConstFold_stddev",
            "value": 0.4389375894741362,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.43921422127774645 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ConstFold_cv",
            "value": 0.03349271700958884,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.033515383800746965 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/0_mean",
            "value": 13.556377576599592,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.55585218353209 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/0_median",
            "value": 13.399109039675869,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.398634979266646 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/0_stddev",
            "value": 0.23179375582081785,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.23156864155879392 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/0_cv",
            "value": 0.017098502495307432,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.017082558766767012 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/1_mean",
            "value": 14.012799347978694,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.012364957566225 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/1_median",
            "value": 14.00882269565231,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.00830005077402 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/1_stddev",
            "value": 0.007591236566344495,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.007880982379892622 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Arith/1_cv",
            "value": 0.0005417359071397471,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0005624305678419507 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/10_mean",
            "value": 12.784623549731219,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.770373607596657 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/10_median",
            "value": 12.760288385791675,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.759677717369467 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/10_stddev",
            "value": 0.06268873255158999,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.03366741910092831 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/10_cv",
            "value": 0.0049034476695958675,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.002636369156882045 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/100_mean",
            "value": 12.960460683929577,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.959440126509486 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/100_median",
            "value": 12.769883391367296,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.76974040138126 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/100_stddev",
            "value": 0.43087062376460494,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.431007765969301 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/100_cv",
            "value": 0.03324500835829595,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.033258208823978674 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/1000_mean",
            "value": 12.756346388874261,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.755657730811068 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/1000_median",
            "value": 12.755183520525597,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.754306470193582 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/1000_stddev",
            "value": 0.0093558343951013,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009207207096192 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_LongChain/1000_cv",
            "value": 0.0007334258658310819,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0007218135897416056 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfThenElse_mean",
            "value": 13.361474790551682,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.36116594654068 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfThenElse_median",
            "value": 13.066189654869357,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.066041095927542 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfThenElse_stddev",
            "value": 0.41235494762192104,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.4121268030286765 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfThenElse_cv",
            "value": 0.0308614845356375,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.030845122699443728 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/2_mean",
            "value": 13.782822496322018,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.782185755080363 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/2_median",
            "value": 13.688208770690864,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.687326496316183 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/2_stddev",
            "value": 0.17745539591934345,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.1776462574030959 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/2_cv",
            "value": 0.012875112914404716,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.012889556167650135 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/8_mean",
            "value": 13.87073059554803,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.8700197142431 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/8_median",
            "value": 13.694614253963612,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.693894672104202 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/8_stddev",
            "value": 0.40772939718812584,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.4080371025809878 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/8_cv",
            "value": 0.029394947467222186,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.029418638977273783 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/32_mean",
            "value": 13.692610057551686,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.691984669872658 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/32_median",
            "value": 13.69281905846976,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.6922783498363 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/32_stddev",
            "value": 0.011449612453298494,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.011610772779773482 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_Switch/32_cv",
            "value": 0.0008361891856391437,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.000847997792848936 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfBlock_mean",
            "value": 11.201933226153772,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11.20103719447726 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfBlock_median",
            "value": 11.203591368901137,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11.202715543352708 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfBlock_stddev",
            "value": 0.00570704339854612,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006030716009052511 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfBlock_cv",
            "value": 0.000509469507033087,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.000538406926460881 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_RefNode_mean",
            "value": 11.204869639846537,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11.204228618025425 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_RefNode_median",
            "value": 11.204815202223356,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11.203592315304729 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_RefNode_stddev",
            "value": 0.008051762943568625,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.007987532989158604 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_RefNode_cv",
            "value": 0.0007185949682926345,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0007129034279350761 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FunctionCall_Builtin_mean",
            "value": 13.090696753684403,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.090063306481184 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FunctionCall_Builtin_median",
            "value": 13.070708527643182,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.06934762996384 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FunctionCall_Builtin_stddev",
            "value": 0.06234808073820488,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.06221864812245731 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FunctionCall_Builtin_cv",
            "value": 0.004762777865177947,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004753120490383836 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/16_mean",
            "value": 17.122417242548256,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 17.121471681010764 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/16_median",
            "value": 17.12441632656346,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 17.123120917669308 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/16_stddev",
            "value": 0.0059464076597437725,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005903868912428717 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/16_cv",
            "value": 0.000347287861025094,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00034482251423378717 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/256_mean",
            "value": 31.952819488012153,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 31.9514604006321 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/256_median",
            "value": 31.447528852991418,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 31.446309882535893 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/256_stddev",
            "value": 1.101450830631041,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.1010379952022968 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/256_cv",
            "value": 0.03447116242885158,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.03445970798819935 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/4096_mean",
            "value": 357.5722926099355,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 357.555938262287 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/4096_median",
            "value": 357.6112113384594,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 357.6084749838258 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/4096_stddev",
            "value": 0.5883164719721405,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.5873415522266022 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum/4096_cv",
            "value": 0.001645307771690008,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0016426564052636563 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/16_mean",
            "value": 21.74196264504358,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 21.74082671284556 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/16_median",
            "value": 21.706301795051047,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 21.705506852682756 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/16_stddev",
            "value": 0.09664010970736482,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0961668529678354 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/16_cv",
            "value": 0.00444486596196942,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004423330089421818 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/256_mean",
            "value": 119.17661443088232,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 119.16988869421718 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/256_median",
            "value": 119.16194076630173,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 119.16091284710433 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/256_stddev",
            "value": 0.425916170742219,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.42373245301601525 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/256_cv",
            "value": 0.003573823377817411,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0035557006695146574 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/4096_mean",
            "value": 1567.312650480672,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1567.217754469488 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/4096_median",
            "value": 1566.8615858863818,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1566.8450105560883 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/4096_stddev",
            "value": 2.690858323570502,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.7059071481377623 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax/4096_cv",
            "value": 0.0017168612291524952,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0017265674412001077 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/16_mean",
            "value": 18.881788622306555,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 18.880752069852544 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/16_median",
            "value": 18.482955373834507,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 18.481266005230758 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/16_stddev",
            "value": 0.9665166292598523,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.9663216985512346 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/16_cv",
            "value": 0.051187768732779346,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.05118025463054456 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/256_mean",
            "value": 221.4659793422532,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 221.45382832720094 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/256_median",
            "value": 221.43968736398188,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 221.43774189718405 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/256_stddev",
            "value": 0.16492611129196036,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.167038880127865 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/256_cv",
            "value": 0.0007447017902333604,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0007542830999564519 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/4096_mean",
            "value": 3808.976692978079,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3808.831124115421 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/4096_median",
            "value": 3809.3300217758056,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3809.0893032117738 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/4096_stddev",
            "value": 0.7436449241570139,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.7201748647507387 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAvg/4096_cv",
            "value": 0.0001952348318455026,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.000189080282449644 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/256_mean",
            "value": 178.78307787926516,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 178.77593061007803 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/256_median",
            "value": 178.74930255296368,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 178.7320424925862 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/256_stddev",
            "value": 0.15128913379467995,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.15112812249031413 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/256_cv",
            "value": 0.0008462161832611906,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0008453493821824057 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/4096_mean",
            "value": 2568.9366168632837,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2568.81125205218 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/4096_median",
            "value": 2568.008117106291,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2567.9017325665563 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/4096_stddev",
            "value": 2.2934406422010616,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.3059707025418845 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin/4096_cv",
            "value": 0.0008927587497278902,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0008976800847861762 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/256_mean",
            "value": 417.5081122241053,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 417.49098993444693 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/256_median",
            "value": 417.25674107406894,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 417.2260536105535 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/256_stddev",
            "value": 10.881861543591695,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 10.876149447975441 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/256_cv",
            "value": 0.026063832593869823,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.026051219571668307 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/4096_mean",
            "value": 5330.756566575639,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5330.500788595723 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/4096_median",
            "value": 5321.609417651176,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5321.17430618745 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/4096_stddev",
            "value": 32.523862346914065,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 32.482014379361694 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMedian/4096_cv",
            "value": 0.006101171933237738,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006093614027570346 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/256_mean",
            "value": 15.270374575187802,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.270030139236876 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/256_median",
            "value": 15.265408291390742,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.264669333794558 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/256_stddev",
            "value": 0.018743149161443585,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01880488193433569 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/256_cv",
            "value": 0.0012274190832161085,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0012314895100315414 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/4096_mean",
            "value": 15.2703850622202,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.269716193248218 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/4096_median",
            "value": 15.268904636150353,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 15.267481570995963 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/4096_stddev",
            "value": 0.017223306545912268,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01738653458539358 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortedMedian/4096_cv",
            "value": 0.001127889472055535,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0011386285354197578 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/256_mean",
            "value": 9247.919371555095,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9247.53481202757 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/256_median",
            "value": 9216.282239167758,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9216.197808363966 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/256_stddev",
            "value": 84.89544300502854,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 85.08279366100304 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/256_cv",
            "value": 0.00917995060231076,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009200591875614492 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/4096_mean",
            "value": 196062.74363638536,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 196055.68881118827 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/4096_median",
            "value": 195805.99440563467,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 195803.91188811406 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/4096_stddev",
            "value": 1122.9950403975413,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1125.9515634748059 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountDistinct/4096_cv",
            "value": 0.005727732967361861,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005743019089638124 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/256_mean",
            "value": 30.179304643657456,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 30.178368506626214 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/256_median",
            "value": 30.012481609297037,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 30.012205326795605 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/256_stddev",
            "value": 0.2726367304868216,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.272863617296747 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/256_cv",
            "value": 0.009033897026653975,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009041695452715902 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/4096_mean",
            "value": 344.83879890210443,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 344.81905367439293 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/4096_median",
            "value": 344.21755808769205,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 344.2140978327659 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/4096_stddev",
            "value": 1.1985809448303706,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.1994696320267941 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCountBits/4096_cv",
            "value": 0.003475771718978272,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0034785480072671215 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/256_mean",
            "value": 14.629214631370346,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.628593676063335 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/256_median",
            "value": 14.622739276965158,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.621988508059292 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/256_stddev",
            "value": 0.015182090730735379,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01530201610997014 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/256_cv",
            "value": 0.0010377926029042917,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0010460346666821925 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/4096_mean",
            "value": 14.627220773025801,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.626737592459046 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/4096_median",
            "value": 14.623135081096361,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.622488111569414 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/4096_stddev",
            "value": 0.009732017661915414,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009686707028176873 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListGetAt/4096_cv",
            "value": 0.0006653360753166673,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0006622602591278418 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLen_mean",
            "value": 12.446958375050224,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.44648246304737 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLen_median",
            "value": 12.444300508751402,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.443995204533007 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLen_stddev",
            "value": 0.007451140621741357,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.007721713136141828 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLen_cv",
            "value": 0.0005986314404872662,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0006203932041897772 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/256_mean",
            "value": 12027.418034516902,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12026.960681885505 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/256_median",
            "value": 12049.133592137909,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12048.68163213653 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/256_stddev",
            "value": 191.74899595480295,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 191.8170129755827 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/256_cv",
            "value": 0.015942656637069723,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.01594891827196952 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/4096_mean",
            "value": 182987.33443054202,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 182976.77669519285 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/4096_median",
            "value": 182922.63067807985,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 182908.39302172072 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/4096_stddev",
            "value": 846.8768401319014,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 850.772192210817 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListUnique/4096_cv",
            "value": 0.004628062607542695,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004649618424681586 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/256_mean",
            "value": 1003.4300690095206,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1003.3880203060766 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/256_median",
            "value": 998.3553859675319,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 998.3469646571745 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/256_stddev",
            "value": 12.161034681425868,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.159945030966965 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/256_cv",
            "value": 0.012119464083261873,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.012118885999114936 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/4096_mean",
            "value": 25018.7472431072,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 25017.39575725045 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/4096_median",
            "value": 24996.490959542272,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 24995.57697815999 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/4096_stddev",
            "value": 112.25749921515838,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 111.3337725337049 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortAsc/4096_cv",
            "value": 0.004486935261959846,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0044502542796221525 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/256_mean",
            "value": 823.853086678211,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 823.8216741215608 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/256_median",
            "value": 821.7117567101802,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 821.6774180232278 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/256_stddev",
            "value": 14.034988736634592,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.03104576945637 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/256_cv",
            "value": 0.017035790681107832,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.017031654070545837 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/4096_mean",
            "value": 18978.317990387666,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 18977.46980838267 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/4096_median",
            "value": 18984.78461643222,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 18984.00798970901 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/4096_stddev",
            "value": 79.5960524495894,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 80.05739447497731 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSortDesc/4096_cv",
            "value": 0.004194051996067514,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004218549431685282 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/256_mean",
            "value": 44.499536728290465,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 44.49718852802299 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/256_median",
            "value": 43.851536779433964,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 43.851079613936214 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/256_stddev",
            "value": 1.449311869578959,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.449935972602605 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/256_cv",
            "value": 0.03256914512230333,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.03258488953048076 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/4096_mean",
            "value": 425.11057646760025,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 425.0965932447367 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/4096_median",
            "value": 425.85137614387315,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 425.8472792092318 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/4096_stddev",
            "value": 3.561057427974906,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3.5643478043038557 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListTruncate/4096_cv",
            "value": 0.008376779184289035,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.008384795034694125 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/256_mean",
            "value": 5046.334218837381,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5046.194134738621 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/256_median",
            "value": 5046.163974662273,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5045.928931078656 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/256_stddev",
            "value": 10.993979515965643,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 10.986562333465132 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/256_cv",
            "value": 0.0021786070916441466,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.002177197713784392 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/4096_mean",
            "value": 79460.2092413014,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 79456.45681688531 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/4096_median",
            "value": 79698.04706217205,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 79692.36081004066 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/4096_stddev",
            "value": 575.6970093736389,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 576.9431734296973 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCastStringList/4096_cv",
            "value": 0.007245098079535464,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00726112384748436 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/256_mean",
            "value": 736.9122761576698,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 736.8731952440007 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/256_median",
            "value": 737.1300398264464,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 737.0704529361699 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/256_stddev",
            "value": 0.8478671732719822,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.8409942129015248 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/256_cv",
            "value": 0.001150567307268705,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0011413011333965628 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/4096_mean",
            "value": 11593.724627237678,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11593.36018058307 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/4096_median",
            "value": 11592.891401592331,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11592.28996852243 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/4096_stddev",
            "value": 3.2284141579841665,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3.433880512885538 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMurmurHash3X8632/4096_cv",
            "value": 0.00027846220794303697,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00029619372290672996 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/256_mean",
            "value": 229.56962385170323,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 229.55896301517336 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/256_median",
            "value": 230.42865641716872,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 230.41395729989398 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/256_stddev",
            "value": 4.674350350913582,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4.677242858382835 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/256_cv",
            "value": 0.020361362590083372,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.020374908463381056 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/4096_mean",
            "value": 33659.67493261266,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33658.85077653689 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/4096_median",
            "value": 35931.22808368155,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 35930.873828775555 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/4096_stddev",
            "value": 5083.702054122488,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5083.42023196342 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListConcat/4096_cv",
            "value": 0.15103241681032753,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.15102774202579133 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/256_mean",
            "value": 100.51985196602244,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 100.515272111227 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/256_median",
            "value": 100.48870576270723,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 100.48760700574434 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/256_stddev",
            "value": 0.11328068124300472,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.11540248952099574 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/256_cv",
            "value": 0.0011269483492802564,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0011481090096766091 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/4096_mean",
            "value": 1298.1978827307996,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1298.1669161655986 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/4096_median",
            "value": 1297.4230557369965,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1297.3486319219776 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/4096_stddev",
            "value": 1.4466233450945354,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.4589686868688143 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListIn/4096_cv",
            "value": 0.001114331924537974,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0011238683321079979 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/256_mean",
            "value": 1394.8981166267497,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1394.788099351317 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/256_median",
            "value": 1394.8153965639349,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1394.7485101786183 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/256_stddev",
            "value": 0.6264998319547814,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.582093963264388 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/256_cv",
            "value": 0.0004491366247377491,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00041733505149284413 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/4096_mean",
            "value": 22087.54897425871,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 22086.804657425942 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/4096_median",
            "value": 22085.094891087585,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 22083.767049504997 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/4096_stddev",
            "value": 13.997882700880421,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.468849714470016 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListExp/4096_cv",
            "value": 0.0006337454063913496,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.000655090219653179 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/256_mean",
            "value": 1854.556037628383,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1854.499132938326 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/256_median",
            "value": 1846.4837333866599,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1846.3937330520996 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/256_stddev",
            "value": 18.27684133543216,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 18.29535538100852 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/256_cv",
            "value": 0.00985510330483445,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009865389018554454 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/4096_mean",
            "value": 29387.466968040837,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 29386.35595037844 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/4096_median",
            "value": 29375.811396128887,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 29375.539529015823 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/4096_stddev",
            "value": 42.521124080547985,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 42.221651077377146 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog/4096_cv",
            "value": 0.00144691354742449,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.001436777365273608 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/256_mean",
            "value": 2031.3453639183288,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2031.2656250450777 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/256_median",
            "value": 2030.5448243523265,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2030.4639039169608 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/256_stddev",
            "value": 3.978312073538009,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3.964523598309597 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/256_cv",
            "value": 0.0019584616895790244,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0019517504502748705 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/4096_mean",
            "value": 32296.238235296933,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 32295.319337656 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/4096_median",
            "value": 32280.197197779613,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 32279.046317739896 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/4096_stddev",
            "value": 86.53845258135712,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 86.6875255662313 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog2/4096_cv",
            "value": 0.0026795211241282656,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0026842132960473494 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/256_mean",
            "value": 2072.3395160339,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2072.2446533836173 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/256_median",
            "value": 2069.530815255182,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2069.4595319500045 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/256_stddev",
            "value": 13.415981978849914,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.401673023690481 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/256_cv",
            "value": 0.006473833980894108,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0064672252872303776 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/4096_mean",
            "value": 33485.55079625426,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33484.30761124154 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/4096_median",
            "value": 33000.95725995163,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33000.624121780296 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/4096_stddev",
            "value": 1024.6143663879288,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1024.2772235596706 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListLog10/4096_cv",
            "value": 0.030598701291260928,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.030589768659746594 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/256_mean",
            "value": 454.102527137986,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 454.08757919552227 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/256_median",
            "value": 451.50270593332255,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 451.4818961564013 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/256_stddev",
            "value": 5.256677331613071,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5.260561181128613 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/256_cv",
            "value": 0.011575970221402776,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.011584904371197313 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/4096_mean",
            "value": 7130.085563182829,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7129.806687007348 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/4096_median",
            "value": 7126.396109839033,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7126.319196542067 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/4096_stddev",
            "value": 9.522587498276534,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9.742077228347139 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListCeil/4096_cv",
            "value": 0.0013355502418439013,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0013663872887465704 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/256_mean",
            "value": 452.38338268229825,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 452.3583914536205 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/256_median",
            "value": 451.0279485590214,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 450.9827819694427 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/256_stddev",
            "value": 2.9120733935907483,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.919439983797475 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/256_cv",
            "value": 0.006437180287932574,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00645382077342717 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/4096_mean",
            "value": 7090.009809291444,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7089.64503630751 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/4096_median",
            "value": 7068.68574587377,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 7068.401919646351 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/4096_stddev",
            "value": 47.93904900818214,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 47.9424508563108 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListFloor/4096_cv",
            "value": 0.0067614926209774925,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006762320343372311 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/256_mean",
            "value": 720.49961344826,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 720.4405668035149 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/256_median",
            "value": 718.13259694457,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 718.079106102093 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/256_stddev",
            "value": 5.89954001864196,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5.927809117849505 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/256_cv",
            "value": 0.008188123780396189,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00822803349921048 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/4096_mean",
            "value": 11329.272902674888,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11328.560581267666 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/4096_median",
            "value": 11301.118899339319,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11301.016200594535 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/4096_stddev",
            "value": 49.84331597042159,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 50.27623374819183 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListRound/4096_cv",
            "value": 0.004399515873490291,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004438007228502273 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_Scalar/4096_mean",
            "value": 1847.0943189245445,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1847.0052208483692 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_Scalar/4096_median",
            "value": 1868.6582931838507,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1868.5848118324564 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_Scalar/4096_stddev",
            "value": 50.18759268253878,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 50.18989567872245 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_Scalar/4096_cv",
            "value": 0.027171104457600247,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.027173662051517732 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_LL/4096_mean",
            "value": 2117.322295010676,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2117.232035624595 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_LL/4096_median",
            "value": 2099.459025471381,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2099.2889117652 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_LL/4096_stddev",
            "value": 44.22685112723386,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 44.22904024652398 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListAdd_LL/4096_cv",
            "value": 0.020888105335428328,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.020890029766376633 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_Scalar/4096_mean",
            "value": 1806.2296294835546,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1806.1746149495652 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_Scalar/4096_median",
            "value": 1755.1999434425657,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1755.092083283966 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_Scalar/4096_stddev",
            "value": 75.55361871229671,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 75.57190458709056 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_Scalar/4096_cv",
            "value": 0.04182946480282208,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.04184086298278575 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_LL/4096_mean",
            "value": 2073.376609292101,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2073.2327020633957 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_LL/4096_median",
            "value": 2069.5380100863886,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2069.518409973421 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_LL/4096_stddev",
            "value": 14.06102558000976,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.054963716896584 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSub_LL/4096_cv",
            "value": 0.0067817035829349495,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00677925044444278 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_Scalar/4096_mean",
            "value": 2510.2431995428865,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2510.054403654877 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_Scalar/4096_median",
            "value": 2499.356459586076,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2499.1760123497224 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_Scalar/4096_stddev",
            "value": 27.801503521899633,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 27.811222514895015 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_Scalar/4096_cv",
            "value": 0.011075223120597345,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.011079928177811298 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_LL/4096_mean",
            "value": 2717.405239162222,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2717.230351680492 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_LL/4096_median",
            "value": 2721.084442279926,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2721.0628348759305 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_LL/4096_stddev",
            "value": 14.73324046565681,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 14.62389679829396 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMul_LL/4096_cv",
            "value": 0.00542180468828384,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005381912795597068 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_Scalar/4096_mean",
            "value": 8706.421801396904,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8706.053080838203 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_Scalar/4096_median",
            "value": 8738.184344470135,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8737.537320982472 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_Scalar/4096_stddev",
            "value": 59.08323470410213,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 59.09785284194866 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_Scalar/4096_cv",
            "value": 0.00678616727420931,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006788133760868229 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_LL/4096_mean",
            "value": 8862.105948228855,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8861.608479185898 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_LL/4096_median",
            "value": 8941.60061434263,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8941.482225706783 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_LL/4096_stddev",
            "value": 112.02333166053617,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 111.93814480599012 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListDiv_LL/4096_cv",
            "value": 0.012640712299645291,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.012631808894390887 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_Scalar/4096_mean",
            "value": 8796.311539554083,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8795.9770786657 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_Scalar/4096_median",
            "value": 8834.34845855326,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8834.073427786645 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_Scalar/4096_stddev",
            "value": 61.39717822933801,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 61.52939643895409 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_Scalar/4096_cv",
            "value": 0.006979877639992098,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006995174713243767 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_LL/4096_mean",
            "value": 8780.044566589557,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8779.626585797403 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_LL/4096_median",
            "value": 8747.66906747367,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8747.26203563259 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_LL/4096_stddev",
            "value": 49.55960727529353,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 49.86557317171226 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMod_LL/4096_cv",
            "value": 0.005644573543952297,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0056796918051593035 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax_Scalar/4096_mean",
            "value": 3416.240469180289,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3416.042902934844 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax_Scalar/4096_median",
            "value": 3400.585620666566,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3400.2893555893206 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax_Scalar/4096_stddev",
            "value": 152.4558584774437,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 152.39539616999082 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMax_Scalar/4096_cv",
            "value": 0.044626793650162686,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.04461167511656909 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin_Scalar/4096_mean",
            "value": 3615.485562053452,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3615.378197825482 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin_Scalar/4096_median",
            "value": 3645.3246983895697,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3645.119058190708 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin_Scalar/4096_stddev",
            "value": 58.4839879708624,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 58.5135711166251 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListMin_Scalar/4096_cv",
            "value": 0.016175970548654557,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.016184633505788935 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_Scalar/4096_mean",
            "value": 1839.5535889345551,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1839.4684448158478 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_Scalar/4096_median",
            "value": 1866.639135911652,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1866.4913815682964 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_Scalar/4096_stddev",
            "value": 87.54362179833392,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 87.53877658279839 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_Scalar/4096_cv",
            "value": 0.04758960126246608,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.04758917002871558 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_LL/4096_mean",
            "value": 2080.3714787975155,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2080.255598988262 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_LL/4096_median",
            "value": 2075.083282030273,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2074.9921450241204 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_LL/4096_stddev",
            "value": 8.459526531100295,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 8.4503291189569 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseAnd_LL/4096_cv",
            "value": 0.004066353830225563,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.004062159055390473 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_Scalar/4096_mean",
            "value": 1767.0919020596393,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1766.9984764585613 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_Scalar/4096_median",
            "value": 1761.8726665709244,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1761.7253639121604 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_Scalar/4096_stddev",
            "value": 58.13530824963294,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 58.07796568609585 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_Scalar/4096_cv",
            "value": 0.03289885952274081,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.03286814700740228 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_LL/4096_mean",
            "value": 2090.6746110113295,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2090.5968686414844 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_LL/4096_median",
            "value": 2092.654585576949,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2092.5880760023215 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_LL/4096_stddev",
            "value": 20.017924677983068,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 20.064303757636445 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseOr_LL/4096_cv",
            "value": 0.009574863813120936,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009597404482230313 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_Scalar/4096_mean",
            "value": 1863.5340595072653,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1863.4709384975954 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_Scalar/4096_median",
            "value": 1866.4917448128745,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1866.4049157023333 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_Scalar/4096_stddev",
            "value": 9.585098919574033,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9.60860936759114 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_Scalar/4096_cv",
            "value": 0.005143506162752088,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005156296870042944 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_LL/4096_mean",
            "value": 2098.3941222146664,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2098.2760602711883 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_LL/4096_median",
            "value": 2076.0033002809087,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2075.920472194087 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_LL/4096_stddev",
            "value": 46.734749054927434,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 46.78477991945704 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListBitwiseXor_LL/4096_cv",
            "value": 0.022271673638507482,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.02229677057527426 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenEqualBitmap/4096_mean",
            "value": 1853.220487255164,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1853.1312539361686 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenEqualBitmap/4096_median",
            "value": 1854.3061553254713,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1854.2437071164945 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenEqualBitmap/4096_stddev",
            "value": 2.5213614970247953,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.5382297341480102 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenEqualBitmap/4096_cv",
            "value": 0.0013605296910780575,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0013696977635861726 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenNotEqualBitmap/4096_mean",
            "value": 1854.3149456146016,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1854.2553602009543 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenNotEqualBitmap/4096_median",
            "value": 1853.9442457105429,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1853.9244222567 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenNotEqualBitmap/4096_stddev",
            "value": 2.1305592058651763,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.165533935434708 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenNotEqualBitmap/4096_cv",
            "value": 0.0011489737549189708,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0011678725497657554 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeBitmap/4096_mean",
            "value": 1855.2850368834766,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1855.1871119248754 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeBitmap/4096_median",
            "value": 1855.0266279250543,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1854.8240327971903 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeBitmap/4096_stddev",
            "value": 0.7377604028996456,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.7366829716286531 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeBitmap/4096_cv",
            "value": 0.0003976534000074412,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0003970936230061977 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeEqualBitmap/4096_mean",
            "value": 1855.943446553653,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1855.8739642140329 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeEqualBitmap/4096_median",
            "value": 1855.7839561112992,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1855.637770486522 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeEqualBitmap/4096_stddev",
            "value": 2.441261611247298,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.4559227292880292 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLargeEqualBitmap/4096_cv",
            "value": 0.0013153750001275828,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.001323324092392297 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessBitmap/4096_mean",
            "value": 1860.4501222188233,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1860.378722699546 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessBitmap/4096_median",
            "value": 1855.929822004778,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1855.9110354466961 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessBitmap/4096_stddev",
            "value": 11.565606595047026,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 11.574519380597298 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessBitmap/4096_cv",
            "value": 0.006216563646035062,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.006221593076382761 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessEqualBitmap/4096_mean",
            "value": 1830.9209696257253,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1830.8240644677237 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessEqualBitmap/4096_median",
            "value": 1826.6635933570142,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1826.6454406056491 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessEqualBitmap/4096_stddev",
            "value": 9.813799205384086,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9.819837946408756 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_GenLessEqualBitmap/4096_cv",
            "value": 0.005360034304151429,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005363616382912075 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfEqual/4096_mean",
            "value": 2398.294874056933,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2398.1792975274498 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfEqual/4096_median",
            "value": 2404.1064072154563,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2403.94220574006 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfEqual/4096_stddev",
            "value": 19.266647784395317,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 19.352837357469532 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfEqual/4096_cv",
            "value": 0.008033477448002061,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00806980419580076 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfNotEqual/4096_mean",
            "value": 2510.388200554181,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2510.305000085955 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfNotEqual/4096_median",
            "value": 2507.3286299695787,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2507.302691161873 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfNotEqual/4096_stddev",
            "value": 9.659483406486313,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 9.6074087385571 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfNotEqual/4096_cv",
            "value": 0.0038478046560105456,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0038271878270680786 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLarge/4096_mean",
            "value": 2404.082175496717,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2403.9988627774246 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLarge/4096_median",
            "value": 2402.3442846838143,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2402.320320552494 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLarge/4096_stddev",
            "value": 4.132803748199047,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4.114593170692665 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLarge/4096_cv",
            "value": 0.0017190775716080305,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0017115620287519314 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLargeEqual/4096_mean",
            "value": 2533.194533655221,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2533.1013006131316 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLargeEqual/4096_median",
            "value": 2504.8560978016385,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2504.8274466156495 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLargeEqual/4096_stddev",
            "value": 76.05213528436997,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 76.0416114920421 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLargeEqual/4096_cv",
            "value": 0.030022224615585326,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.030019175101144352 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLess/4096_mean",
            "value": 2507.7043227770223,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2507.618723358488 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLess/4096_median",
            "value": 2507.3189539906357,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2507.17812497184 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLess/4096_stddev",
            "value": 1.1279452803468755,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.164401245260008 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLess/4096_cv",
            "value": 0.0004497919751152293,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0004643454104141117 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLessEqual/4096_mean",
            "value": 2570.094803369914,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2570.0020929753764 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLessEqual/4096_median",
            "value": 2610.4891546636527,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2610.3931624695106 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLessEqual/4096_stddev",
            "value": 58.074841409494454,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 58.104401995445734 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfLessEqual/4096_cv",
            "value": 0.022596381010282808,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.022608698317508502 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfByBitmap/4096_mean",
            "value": 4146.103735144537,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4145.950450793206 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfByBitmap/4096_median",
            "value": 4101.01757801122,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 4100.872680170965 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfByBitmap/4096_stddev",
            "value": 62.95242854674668,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 62.97598990637467 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_IfByBitmap/4096_cv",
            "value": 0.015183515070577969,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.015189759417970386 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FilterByBitmap/4096_mean",
            "value": 1821.3404071808181,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1821.2267415504634 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FilterByBitmap/4096_median",
            "value": 1807.253749206627,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1807.107912947574 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FilterByBitmap/4096_stddev",
            "value": 29.99806996078067,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 29.988308534192374 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_FilterByBitmap/4096_cv",
            "value": 0.016470325833935408,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.016465993964410192 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaReuse_mean",
            "value": 12.983168368974495,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.982781942214553 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaReuse_median",
            "value": 12.757292760461029,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 12.757175825587327 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaReuse_stddev",
            "value": 0.3322089841198163,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.3321784208357236 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaReuse_cv",
            "value": 0.02558766663718901,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.025586074102933128 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaFresh_mean",
            "value": 13.079735067572292,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.079160009365022 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaFresh_median",
            "value": 13.080017481663935,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 13.079888742171292 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaFresh_stddev",
            "value": 0.008508510342675215,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.008168426404368495 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ArenaFresh_cv",
            "value": 0.0006505109085710606,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0006245375389948349 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaReuse_mean",
            "value": 31.582283437242495,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 31.58084110840837 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaReuse_median",
            "value": 31.496381805127875,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 31.49342358565055 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaReuse_stddev",
            "value": 0.23376174555566806,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.23350695590056292 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaReuse_cv",
            "value": 0.007401673347026303,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.007393943533644261 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaFresh_mean",
            "value": 31.54723055476571,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 31.545697053581534 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaFresh_median",
            "value": 31.55111869926906,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 31.549910938195545 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaFresh_stddev",
            "value": 0.01930830227574299,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.02020523658837257 ns\nthreads: 1"
          },
          {
            "name": "BM_Execute_ListSum_ArenaFresh_cv",
            "value": 0.0006120442883955835,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0006405068987397307 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/2_mean",
            "value": 33.94054138890337,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33.93835715459523 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/2_median",
            "value": 33.939885963606756,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 33.93953876572677 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/2_stddev",
            "value": 0.027952422793414495,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.027970162968769297 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/2_cv",
            "value": 0.0008235703276834986,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0008241460493022761 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/8_mean",
            "value": 102.33139840801125,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 102.32468412632309 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/8_median",
            "value": 102.11638122370776,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 102.11124438318787 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/8_stddev",
            "value": 0.3808528941721974,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.3782109448213171 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/8_cv",
            "value": 0.0037217598908760876,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.003696184826277144 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/32_mean",
            "value": 553.914548738302,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 553.8883846481059 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/32_median",
            "value": 554.6057228341439,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 554.578573406778 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/32_stddev",
            "value": 5.1445513577387185,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 5.129804489316816 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_All/32_cv",
            "value": 0.009287626348607197,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.009261440809190943 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/2_mean",
            "value": 24.894049057575554,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 24.892854286133293 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/2_median",
            "value": 24.892895507853915,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 24.892583631866533 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/2_stddev",
            "value": 0.012970224361742189,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.013580278200981153 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/2_cv",
            "value": 0.0005210170644295085,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00054554925862986 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/8_mean",
            "value": 95.10888138395451,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 95.10386964551552 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/8_median",
            "value": 92.99722953879032,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 92.99637767676046 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/8_stddev",
            "value": 2.9394680363654007,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.9370340932374033 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/8_cv",
            "value": 0.030906346427299145,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.03088238264315352 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/32_mean",
            "value": 524.4690230610594,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 524.453033455792 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/32_median",
            "value": 523.66411845535,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 523.6268992400336 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/32_stddev",
            "value": 3.1547101819056733,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 3.158505727210332 ns\nthreads: 1"
          },
          {
            "name": "BM_BatchExecute_AtLoop/32_cv",
            "value": 0.006015055309641038,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0060224758476424625 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_i64_mean",
            "value": 0.3519835692055613,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.35196476790261205 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_i64_median",
            "value": 0.3519083933546986,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.3518954393998114 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_i64_stddev",
            "value": 0.00038003477820692594,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00038881449295005894 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_i64_cv",
            "value": 0.0010796946546814022,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0011046971981515013 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_f64_mean",
            "value": 0.31163739278176283,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.3116256842298972 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_f64_median",
            "value": 0.31150096436128705,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.31148441207978095 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_f64_stddev",
            "value": 0.00026092533929622313,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.00026140294855963225 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_Arith_f64_cv",
            "value": 0.0008372722444092164,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0008388363404820834 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/16_mean",
            "value": 2.179466100894294,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.179371926827618 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/16_median",
            "value": 2.178836172926569,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 2.1786720445860683 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/16_stddev",
            "value": 0.0016448383219767916,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.001659552842122947 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/16_cv",
            "value": 0.0007546978231512157,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0007614821599260752 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/256_mean",
            "value": 20.904634369600476,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 20.90371193207222 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/256_median",
            "value": 20.878421847578355,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 20.87825320453873 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/256_stddev",
            "value": 0.053635118628251474,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.052986562525367556 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/256_cv",
            "value": 0.002565704698774722,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.0025347920358618765 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/4096_mean",
            "value": 330.3063286230927,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 330.29426470362813 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/4096_median",
            "value": 329.45802829872287,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 329.4398125792344 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/4096_stddev",
            "value": 1.7259949465774802,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 1.7258758198504172 ns\nthreads: 1"
          },
          {
            "name": "BM_Native_ListSum_i64/4096_cv",
            "value": 0.005225437107948923,
            "unit": "ns/iter",
            "extra": "iterations: 5\ncpu: 0.005225267297326641 ns\nthreads: 1"
          }
        ]
      }
    ]
  }
}