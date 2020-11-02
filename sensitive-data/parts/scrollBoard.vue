<template>
  <div id="scroll-board">
    <dv-scroll-board :config="config" />
  </div>
</template>

<script>
export default {
  name: 'ScrollBoard',
  data () {
    return {
      config: {
        header: ['时间', '交易品种', '价格', '是否正常'],
        data: [
          ['2019-07-01 19:25:00', '原油', '5', 'xxxxxxx'],
          ['2019-07-02 17:25:00', '天然气', '13', 'xxxxxxx'],
          ['2019-07-03 16:25:00', '玉米', '6', 'xxxxxxx'],
          ['2019-07-04 15:25:00', '大豆', '2', 'xxxxxxx'],
          ['2019-07-05 14:25:00', '原木', '1', 'xxxxxxx'],
          ['2019-07-06 13:25:00', '牛肉', '3', 'xxxxxxx'],
          ['2019-07-07 12:25:00', '钢材', '4', 'xxxxxxx'],
          ['2019-07-08 11:25:00', '铁矿石', '2', 'xxxxxxx'],
          ['2019-07-09 10:25:00', '钒', '5', 'xxxxxxx'],
          ['2019-07-10 09:25:00', '黄金', '3', 'xxxxxxx']
        ],
        columnWidth: [180, 180, 100],
        align: ['center', 'left', 'left', 'right'],
        rowNum: 7,
        headerBGC: '#1981f6',
        headerHeight: 45,
        oddRowBGC: 'rgba(0, 44, 81, 0.8)',
        evenRowBGC: 'rgba(10, 29, 50, 0.8)'
      }
    }
  }
}
</script>

<style lang="less">
#scroll-board {
  width: 60%;
  box-sizing: border-box;
  margin-left: 20px;
  height: 100%;
  overflow: hidden;

  .header {
    height: auto;
  }
}
</style>
