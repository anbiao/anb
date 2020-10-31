<template>
  <div id="data-view">
    <dv-full-screen-container>
      <top-header />
      <div class="main-content">
        <div class="decoration-content-top">
          <redirect-button/>
        </div>
        <dv-border-box-1 class="main-container">
          <dv-border-box-3 class="left-chart-container">

            <Left-Chart-1 />
            <Left-Chart-2 />
            <Left-Chart-3 />

          </dv-border-box-3>
          <div class="right-main-container">
            <div class="rmctc-left-container">

              <dv-border-box-3 class="rmctclc-top">

                <Center-Cmp />

              </dv-border-box-3>
              <div class="rmctclc-bottom">

                <Scroll-Board />

              </div>
            </div>

            <div class="rmctc-right-container">
                <dv-border-box-3 class="rmctc-chart-1">

                  <Right-Chart-1 />

                </dv-border-box-3>

                <dv-border-box-3 class="rmctc-chart-2">

                  <Right-Chart-2 />

                </dv-border-box-3>

                <dv-border-box-3 class="rmctc-chart-3">

                  <Right-Chart-3 />

                </dv-border-box-3>
              </div>

          </div>
        </dv-border-box-1>
      </div>
    </dv-full-screen-container>
  </div>
</template>

<script>
import LeftChart1 from './parts/LeftChart1'
import LeftChart2 from './parts/LeftChart2'
import LeftChart3 from './parts/LeftChart3'

import CenterCmp from './parts/CenterCmp'

import RightChart1 from './parts/RightChart1'
import RightChart2 from './parts/RightChart2'
import RightChart3 from './parts/RightChart3'

import ScrollBoard from './parts/ScrollBoard'

import topHeader from './parts/topHeader'
import redirectButton from './parts/redirectButton'

export default {
  name: 'DataView',
  components: {
    topHeader,
    redirectButton,
    LeftChart1,
    LeftChart2,
    LeftChart3,
    CenterCmp,
    RightChart1,
    RightChart2,
    RightChart3,
    ScrollBoard
  },
  data () {
    return {}
  }
}
</script>

<style lang="less">
#data-view {
  width: 100%;
  height: 100%;
  background-color: #030409;
  color: #fff;

  #dv-full-screen-container {
    background-image: url('img/bg.png');
    background-size: 100% 100%;
    box-shadow: 0 0 3px blue;
    display: flex;
    flex-direction: column;
  }

  .main-content {
    flex: 1;
    display: flex;
    padding-top: 35px;
    flex-direction: column;

    .decoration-content-top {
      display: flex;
      justify-content: space-between;
    }
  }

  .main-container {
    height: 100%;

    .border-box-content {
      padding: 20px;
      box-sizing: border-box;
      display: flex;
    }
  }

  .left-chart-container {
    width: 22%;
    padding: 10px;
    box-sizing: border-box;

    .border-box-content {
      flex-direction: column;
    }
  }

  .right-main-container {
    width: 78%;
    padding-left: 5px;
    box-sizing: border-box;
    display: flex;
  }

  .rmctc-left-container {
    width: 65%;
    height: 100%;
  }

  .rmctclc-top{
    width: 100%;
    height: 65%;
  }

  .rmctclc-bottom{
    width: 100%;
    height: 35%;
  }

  .rmctc-right-container {
    width: 35%;
    height: 100%;
  }

  .rmc-bottom-container {
    height:35%;
    width:65%;
  }

  .rmctc-chart-1, .rmctc-chart-2, .rmctc-chart-3 {
    height: 30%;
  }
}
</style>
