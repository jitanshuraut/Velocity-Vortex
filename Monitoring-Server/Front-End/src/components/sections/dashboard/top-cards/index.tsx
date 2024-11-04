import Grid from '@mui/material/Grid';
import Spent from './spent';
import Total_Trades from './Total_Trades';
import Earnings from './earnings';
import Activity from './activity';

const TopCards = () => {
  return (
    <Grid container spacing={2.5}>
      <Grid item xs={12} sm={6} xl={3}>
        <Spent balance={200} />
      </Grid>
      <Grid item xs={12} sm={6} xl={3}>
        <Total_Trades trades={30} />
      </Grid>
      <Grid item xs={12} sm={6} xl={3}>
        <Earnings earnings={450.3} />
      </Grid>
      <Grid item xs={12} sm={6} xl={3}>
        <Activity />
      </Grid>
    </Grid>
  );
};

export default TopCards;
