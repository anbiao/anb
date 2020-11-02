<template>
  <div id="data-view">
    <dv-full-screen-container>
      <top-header />
      <div class="main-content">
        <div class="decoration-content-top">
          <redirect-button/>
        </div>
        <digital-flop />
        <div class="block-left-right-content">
          <ranking-board />
          <div class="block-top-bottom-content">
            <div class="block-top-content">
              <rose-chart />
              <scroll-board />
            </div>
            <cards />
          </div>
        </div>
      </div>
    </dv-full-screen-container>
  </div>
</template>

<script>
import topHeader from './parts/topHeader'
import redirectButton from './parts/redirectButton'
import digitalFlop from './parts/digitalFlop'
import rankingBoard from './parts/rankingBoard'
import roseChart from './parts/roseChart'
import waterLevelChart from './parts/waterLevelChart'
import scrollBoard from './parts/scrollBoard'
import cards from './parts/cards'

export default {
  name: 'DataView',
  components: {
    topHeader,
    redirectButton,
    digitalFlop,
    rankingBoard,
    roseChart,
    waterLevelChart,
    scrollBoard,
    cards
  },
  data () {
    return {}
  },
  methods: {}
}
</script>

<style lang="less">
#data-view {
  width: 100%;
  height: 100%;
  background-color: #030409;
  color: #fff;

  #dv-full-screen-container {
    background-image: url('./img/bg.png');
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

  .block-left-right-content {
    flex: 1;
    display: flex;
    margin-top: 20px;
  }

  .block-top-bottom-content {
    flex: 1;
    display: flex;
    flex-direction: column;
    box-sizing: border-box;
    padding-left: 20px;
  }

  .block-top-content {
    height: 55%;
    display: flex;
    box-sizing: border-box;
    padding-bottom: 20px;
  }
}
</style>
