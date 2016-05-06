class SLOG_global
{
    public static final short MinRectype4VarDiagRec   = 0;
    public static final short MaxRectype4VarDiagRec   = 63;
    public static final short MinRectype4FixDiagRec   = 64;
    public static final short MaxRectype4FixDiagRec   = 127;
    public static final short MinRectype4VarOffdRec   = 128;
    public static final short MaxRectype4VarOffdRec   = 191;
    public static final short MinRectype4FixOffdRec   = 192;
    public static final short MaxRectype4FixOffdRec   = 255;

    public static boolean IsVarRec( short rectype )
    {
        return    (    rectype >= MinRectype4VarDiagRec
                    && rectype <= MaxRectype4VarDiagRec )
               || (    rectype >= MinRectype4VarOffdRec
                    && rectype <= MaxRectype4VarOffdRec );
    }

    public static boolean IsOffDiagRec( short rectype )
    {
        return    (    rectype >= MinRectype4VarOffdRec
                    && rectype <= MaxRectype4VarOffdRec )
               || (    rectype >= MinRectype4FixOffdRec
                    && rectype <= MaxRectype4FixOffdRec );
    }
}
