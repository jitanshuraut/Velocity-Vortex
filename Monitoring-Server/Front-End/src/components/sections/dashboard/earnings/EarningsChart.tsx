import { useMemo } from 'react';
import { SxProps, useTheme } from '@mui/material';
import * as echarts from 'echarts/core';
import ReactEchart from 'components/base/ReactEchart';
import { CanvasRenderer } from 'echarts/renderers';
import { LineChart } from 'echarts/charts';

import {
  TitleComponent,
  TooltipComponent,
  GridComponent,
  LegendComponent,
} from 'echarts/components';

echarts.use([
  LineChart,
  TitleComponent,
  TooltipComponent,
  GridComponent,
  LegendComponent,
  CanvasRenderer,
]);

interface ClientChartProps {
  data: number[];
  sx?: SxProps;
}

const EarningsChart = ({ data, ...rest }: ClientChartProps) => {
  const theme = useTheme();

  const option = useMemo(
    () => ({
      tooltip: {
        trigger: 'axis',
        formatter: 'Earnings: ${c}',
      },
      grid: {
        top: 40,
        bottom: 70,
        left: 0,
        right: 0,
        containerLabel: true,
      },
      xAxis: {
        type: 'category',
        data: ['', '', '', '', '', '', '', '', '', '', '', '', '', '', ''],
        axisTick: {
          show: false,
        },
        axisLine: {
          show: false,
        },
        boundaryGap: 0,
      },
      yAxis: {
        type: 'value',
        min: 10,
        minInterval: 1,
        axisLabel: {
          show: false,
        },
        splitLine: {
          show: false,
        },
      },
      series: [
        {
          data,
          type: 'line',
          smooth: true,
          showSymbol: false,
          symbol: 'none',
          lineStyle: {
            width: 3,
            type: 'solid',
            cap: 'round',
            color: theme.palette.primary.main,
          },
          areaStyle: {
            color: new echarts.graphic.LinearGradient(0, 0, 0, 1, [
              { offset: 0, color: 'rgba(84, 112, 198, 0.5)' },
              { offset: 1, color: 'rgba(84, 112, 198, 0)' },
            ]),
          },
        },
      ],
    }),
    [theme, data],
  );

  return <ReactEchart echarts={echarts} option={option} {...rest} />;
};

export default EarningsChart;
